#ifndef INCLUDED_MASHIRO_BASE_MEMORYMANAGER_H
#define INCLUDED_MASHIRO_BASE_MEMORYMANAGER_H

namespace Mashiro{

//new�������ǂ�N���X
class MemoryManager{
public:
	static MemoryManager instance();

	///���̏����o��(�t�@�C������^����΃t�@�C���ցA�^���Ȃ���΃f�o�O�o�͂�)
	void write( const char* filename = 0 );
	///�S�m�ۗe��(�����\��e�ʂ��܂�)
	int totalSize() const;
};

}

void* operator new( size_t size, const char* filename, int line );
void* operator new[]( size_t size, const char* filename, int line );
void operator delete( void*, const char*, int );
void operator delete[]( void*, const char*, int );
void* operator new( size_t size );
void* operator new[]( size_t size );
void operator delete( void* );
void operator delete[]( void* );

#endif
