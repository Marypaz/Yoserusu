#ifndef INCLUDE_LUA_MANAGER_H_
#define INCLUDE_LUA_MANAGER_H_

#include "lua.hpp"

class LuaManager{
public:
	/*
	* @brif シングルトン生成
	*/
	static void create();
	/*
	* @brif シングルトン破棄
	*/
	static void destory();
	/*
	* @brif インスタンス取得
	* @return LuaManager* mInstance;
	*/
	static LuaManager* instance();
	/*
	* @brif luaスクリプトロード
	*/
	void loadLua( const char* lua );
	/*
	* @brif lua内関数実行
	* @param const char* className - クラスの名前
	* @param const char* functionName - 関数名
	*/
	void runLua( const char* className, const char* functionName );
	/*
	* @brif lua内関数を実行したあとに値を取り出す
	*/
	template< typename Type > Type getLuaRetrun( int numArgs = 1 );
private:
	LuaManager();
	~LuaManager();
	static LuaManager* mInstance;

	lua_State* mLuaState; 
};

#endif