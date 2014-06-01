#ifndef INCLUDE_MASHIRO_GRAPHICS_BITMAP_H_
#define INCLUDE_MASHIRO_GRAPHICS_BITMAP_H_

namespace Mashiro{
namespace Graphics {

class Bitmap {
public:
	//ビットマップを作成
	static Bitmap create( const char* fileName );
	//ビットマップイメージを作成
	static Bitmap create( int width, int height, long stride );

	//unsigned char*からコピー
	void copyFromMemory( unsigned char* byte );

	//サイズ
	int height() const;
	int width() const;

	//以下ユーザは意識しない関数群
	Bitmap();
	Bitmap( const Bitmap& );
	~Bitmap();
	operator void*() const;
	Bitmap& operator=( const Bitmap& );
	void release();
	bool operator==( const Bitmap& ) const;
	bool operator!=( const Bitmap& ) const;
	bool operator<( const Bitmap& ) const;

	class Impl;
	Bitmap( const Impl* );
private:
	friend class Sprite;
	friend class Kinect;
	Impl* mImpl;
};

}
}

#endif