#ifndef INCLUDE_MASHIRO_THREADING_MANAGER_IMPL_H_
#define INCLUDE_MASHIRO_THREADING_MANAGER_IMPL_H_

#include <Windows.h>
#include <process.h>
#undef min
#undef max
#include <list>

#include "Mashiro/Mashiro.h"
#include "Mashiro/Threading/ThreadingManager.h"
#include "Mashiro/Threading/Thread.h"
#include "Mashiro/Threading/Event.h"
#include "Mashiro/Threading/Mutex.h"
#include "Mashiro/Threading/Semaphore.h"

namespace Mashiro {
namespace Threading {
using namespace std;

class ManagerImpl {
public:
	struct Entry {
		Entry() : mThread( 0 ), mFinished( 0 ){ }
		Entry( Thread* t, Event* e ) : mThread( t ), mFinished( e ){}
		~Entry(){
			mThread = 0;
			mFinished = 0;
		}
		Thread* mThread;
		Event* mFinished;
	};
	ManagerImpl( int additionalThreadNumber ) :
	mThreads( 0 ),
	mThreadNumber( 0 ),
	mEndRequest( false ),
	mCoreNumber( 1 )
	{
		mQueueSemaphore = Semaphore::create( 0, 0x7fffffff );
		mLock = Mutex::create();

		//�R�A���擾
#ifdef NDEBUG
		HANDLE process = GetCurrentProcess();
		DWORD processMask;
		DWORD systemMask;
		BOOL succeded = GetProcessAffinityMask( process, &processMask, &systemMask );
		if( succeded != 0 ){
			for( int i = 0; i < 32; ++i ){
				if( processMask & ( 1 << i ) ){
					++mCoreNumber;
				}
			}
		}
#endif
		mThreadNumber = additionalThreadNumber + mCoreNumber + 1; //�Œ�ł��R�A��+1�͍��
		mThreads = NEW HANDLE[ mThreadNumber ];
		//�N��
		for( int i = 0; i < mThreadNumber; ++i ){
			unsigned dummy;
			mThreads[ i ] = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, execute, this, 0, &dummy ) );
			SetThreadPriority( mThreads[ i ], THREAD_PRIORITY_BELOW_NORMAL );
		}
	}
	~ManagerImpl(){
		//�I���҂�
		mLock.lock();
		mEndRequest = true;
		mLock.unlock();
		
		//�X���b�h�I���҂�
		for( int i = 0; i < mThreadNumber; ++i ){
			while( true ){
				mQueueSemaphore.increase(); //�L���[�ɉ��������Ă���悤�Ɍ��������ċN����	
				if( WAIT_OBJECT_0 == WaitForSingleObject( mThreads[ i ], i ) ){
					CloseHandle( mThreads[ i ] );
					mThreads[ i ] = 0;
					break;
				}
			}
		}
		SAFE_DELETE( mThreads );
	}
	static unsigned WINAPI execute( void* p ){
		ManagerImpl* m = reinterpret_cast< ManagerImpl* >( p );
		m->execute();
		return 0;
	}
	void execute(){
		while( true ){
			mQueueSemaphore.decrease(); //�L���[�ǉ���҂�

			mLock.lock();
			if( mEndRequest ){
				mLock.unlock();
				break;
			}
			Entry e = mQueue.front();
			mQueue.pop_front();
			mLock.unlock();

			( *( e.mThread ) )(); //���s
			e.mFinished->set();
		}
	}
	void add( Thread* t, Event* finishFlag ){
		Entry e( t, finishFlag );

		mLock.lock();
		mQueue.push_back( e );
		mLock.unlock();

		mQueueSemaphore.increase(); //�����܂���
	}
	int getCoreNumber() const {
		return mCoreNumber;
	}
private:
	HANDLE* mThreads;
	int mThreadNumber;
	bool mEndRequest;
	Semaphore mQueueSemaphore;
	Mutex mLock;
	list< Entry > mQueue;
	int mCoreNumber;
};
extern ManagerImpl* gManagerImpl;

}
}

#endif