#ifndef INCLUDED_SEQUENCE_GAME_CHILD_H
#define INCLUDED_SEQUENCE_GAME_CHILD_H

namespace Sequence{
namespace Game{
class ParentGame;

class Child{
public:
	virtual ~Child(){} //�Ȃɂ����Ȃ��̂ł���ł���
	virtual void update( ParentGame* ) = 0;
};

} //namespace Game
} //namespace Sequence

#endif