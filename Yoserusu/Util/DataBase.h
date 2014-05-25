#ifndef INCLUDE_DATABASE_H_
#define INCLUDE_DATABASE_H_

//txt�̃f�[�^����擾���Ă���
class DataBase{
public:
	static DataBase* instance();
	static void create();
	static void destory();

	void fileOpen( const char* txt );

	int clearLine() const;
	int gameTime() const;
	int maxDepth() const;
	int minDepth() const;
private:
	DataBase();
	~DataBase();
	static DataBase* mInstance;

	int mClearLine;//�N���A���C����
	int mGameTime;//�Q�[���^�C��
	int mMaxDepth;
	int mMinDepth;
};

#endif