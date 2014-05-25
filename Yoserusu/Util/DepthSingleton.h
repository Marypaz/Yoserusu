#ifndef INCLUDE_DEPTH_SINGLETON_H_
#define INCLUDE_DEPTH_SINGLETON_H_

class DepthSingleton {
public:
	static DepthSingleton* instance();
	static void create();
	static void destory();

	//�[�x�l�Z�b�g
	void setDepthMin( float i );
	void setDepthMax( float i );
	//�[�x�l�Q�b�g
	int getDepthMin() const;
	int getDepthMax() const;
private:
	DepthSingleton();
	~DepthSingleton();
	static DepthSingleton* mInstance;

	//�[�x�l
	float mDepthMax;
	float mDepthMin;
};

#endif