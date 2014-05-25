#include "Mashiro/Mashiro.h"
#include "lua.hpp"
#include "luabind/luabind.hpp"
#include "LuaManager.h"
#include "Util/Sprite.h"
using namespace Mashiro;

LuaManager* LuaManager::mInstance = nullptr;

void LuaManager::create(){
	ASSERT( !mInstance );
	mInstance = new LuaManager();
}

void LuaManager::destory(){
	ASSERT( mInstance );
	SAFE_DELETE( mInstance );
}

LuaManager* LuaManager::instance(){
	return mInstance;
}

LuaManager::LuaManager(){
	//luaVM�N��
	mLuaState = lua_open();

	//Lua�W�����C�u�������J��
	luaL_openlibs( mLuaState );

	luabind::open( mLuaState );
	//Luabind�Ɋ֐��o�^
	luabind::module( mLuaState )[
		luabind::class_<SpriteUtil>("SpriteUtil")
			.def(luabind::constructor<const char*>())
			.def("draw", (void(SpriteUtil::*)(int, int))&SpriteUtil::draw)
	];
}

LuaManager::~LuaManager(){
	//Lua��VM�����
	lua_close( mLuaState );
}

void LuaManager::loadLua( const char* lua ){
	int top = -1;
	int ret = -1;
	while( true ){
		top = lua_gettop(mLuaState);	//�X�^�b�N�̗v�f�����擾
		ret = luaL_dofile(mLuaState, lua);
		if (ret != 0) {
			//�G���[����
			const char* erro = lua_tostring(mLuaState,-1);
			MESSAGE( erro );
			printf("error:%s\n", erro );
		} else {
			break;
		}
	}
	lua_settop(mLuaState,top);
}

void LuaManager::runLua( const char* luascpritName, int numArgs, int numReturn ){
	lua_getglobal(mLuaState, luascpritName);
	int ret = lua_pcall(mLuaState , numArgs,  numReturn, NULL);
	if (ret != 0) {
		//�G���[����
		printf("error:%s\n", lua_tostring(mLuaState,-1) );
		lua_pop(mLuaState,1);
	}
}