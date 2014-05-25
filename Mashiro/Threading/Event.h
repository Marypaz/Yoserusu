#ifndef INCLUDE_MASHIRO_THREADING_EVENT_H_
#define INCLUDE_MASHIRO_THREADING_EVENT_H_

namespace Mashiro {

namespace Threading {

class Event {
public:
	static Event create( bool initState = false );
	//true�҂�
	void wait() const;
	//true�ɂ���
	void set();
	//false�ɂ���
	void reset();
	//�`�F�b�N
	bool isSet() const;

	Event();
	~Event();
	Event( const Event& );
	operator void*() const;
	Event& operator=( const Event& );
	void release();
	bool operator==( const Event& ) const;
	bool operator!=( const Event& ) const;
private:
	class Impl;
	Impl* mImpl;
};

}
}

#endif