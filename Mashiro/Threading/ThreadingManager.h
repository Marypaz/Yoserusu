#ifndef INCLUDED_MASHIRO_THREADING_MANAGER_H
#define INCLUDED_MASHIRO_THREADING_MANAGER_H

namespace Mashiro{
namespace Threading{

class Manager{
public:
	Manager();
	static Manager instance();

	//�R�A���擾
	int getCoreNumber() const;

	//�ȉ����[�U���ĂԂ��Ƃ͂Ȃ�
	static void create( int additionalThreadNumber );
	static void destroy();
};

}
}

#endif