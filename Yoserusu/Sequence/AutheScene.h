#ifndef INCLUDED_SEQUENCE_AUTHE_H
#define INCLUDED_SEQUENCE_AUTHE_H

#include "Sequence/Child.h"

class BackGround;
class Authe;

namespace Sequence{
class Parent;

class AutheScene : public Child{
public:
	AutheScene();
	~AutheScene();
	//�V�[���X�V
	void update( Parent* );
	//�X�V
	void autheUpdate( Parent* );
	//�`��
	void autheDraw();
	//�ړ��̃t���O
	bool moveScene();
private:
	//�V�[���|�b�v�t���O
	int mMoveSceneCount;
	//�F�؃N���X
	Authe* mAuthe;
	//�w�i�N���X
	BackGround* mBack;
};

} //namespace Sequence

#endif