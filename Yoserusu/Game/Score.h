#ifndef SCORE_H_
#define SCORE_H_

#include "Mashiro/Mashiro.h"

class Score{
public:
	static Score* instance();
	static void create();
	static void destory();
	//�X�R�A�����Z
	void Add();
	//�X�R�A���Q�b�g
	int getScore()const;
	void Init();
private:
	Score();
	~Score();
	//�X�R�A
	int mCount;
	static Score* mInstance;
};
#endif
