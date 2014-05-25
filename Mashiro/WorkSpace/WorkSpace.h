#ifndef INCLUDE_MASHIRO_WORKSPACE_H_
#define INCLUDE_MASHIRO_WORKSPACE_H_

#include <string>

namespace Mashiro{

class WorkSpace{
public:
	class Configuration{
	public:
		///���ݒ�
		void setWidth( int );
		///�����ݒ�
		void setHeight( int );
		///�E�B���h�E�^�C�g���ݒ�
		void setTitle( const char* title );
		///���������L�����ݒ�
		void enableVSync( bool = true );
		///�t���X�N���[���A���`�G�C���A�X�ݒ�
		void enableAntiAlias( bool = true );
		///�N�����̃X�N���[���ݒ�
		void enableWindowScreen( bool = true );
		///HALT,ASSERT�ő��������ɗ�O�𓊂���@�\�̗L�����B���蕨�ɑ����R�[�h�͓�����Ȃ��A�Ƃ����l�̂��߂̂��́B
		void enableThrowWhenHalt( bool = true );
		///�ǉ��X���b�h���ݒ�
		void setExtraThreadNumber( int );
		///�R�}���h���C���I�v�V�������擾
		const char* commandLineString() const;
	};
	WorkSpace();
	//�C���X�^���X�擾
	static WorkSpace instance();

	//���[�U�[��`�֐��B���Ȃ��ƃG���[�ł邩���
	void configure( Configuration* );
	//���[�U�[��`
	void update();
	//Kinect����
	void kinectCreate();

	//�E�B���h�E�t���O
	void enableWindowFlag( bool );
	//��ʕ��擾
	int width() const;
	//��ʍ����擾
	int height() const;
	//�^�C�g��
	const char* title() const;
	//�E�B���h�E�ł����H
	bool isWindowScreen() const;
	//�I������
	void requestEnd();
	//�I�����߂��`�F�b�N
	bool isEndRequested() const;
	//�t���[�����[�g�擾
	int frameRate() const;
	//�t���[�����[�g�̐ݒ�
	void setFrameRate( int );
	//���Ԏ擾
	unsigned time() const;
	//�X���[�v
	void sleep( int i );

	void start( void* handle );
	void preUpdate();
	void postUpdate();
	static void create();
	static void destroy();
};

}

#endif