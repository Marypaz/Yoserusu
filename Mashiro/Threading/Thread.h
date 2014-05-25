#ifndef INCLUDE_MASHIRO_THREAD_THREADING_H_
#define INCLUDE_MASHIRO_THREAD_THREADING_H_

namespace Mashiro {

namespace Threading {

class Thread {
public:
	//���g�̏���
	virtual void operator()() = 0;
	//�X���b�h�J�n
	void start();
	//�I���҂�
	void wait();
	//�I��������`�F�b�N
	bool isFinished();
	//���ڂ͌Ă΂Ȃ���
	Thread();
protected:
	~Thread();
private:
	class Impl;
	Impl* mImpl;
};

}
}

#endif
