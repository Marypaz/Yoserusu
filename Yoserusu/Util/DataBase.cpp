#include <fstream>
#include <iostream>
#include <istream>
#include <string>
#include <sstream>
#include "DataBase.h"
#include "Mashiro/Mashiro.h"
using namespace std;

DataBase* DataBase::mInstance = 0;

DataBase::DataBase() : mClearLine( 0 ), mGameTime( 0 ), mMaxDepth( 0 ), mMinDepth( 0 ){
}

DataBase::~DataBase(){

}

void DataBase::fileOpen( const char* txt ){
	//txtロード
	string str;
	ifstream ifs;
	ifs.open( txt );
	if( !ifs.is_open() ){
		//開けない
	}
	string d;
	int i;
	while( getline( ifs, str ) ){
		d = ""; i = 0;

		sscanf( str.c_str(), ":%s :%d", d.c_str(), &i );
		//データ取り込みまっすｗ
		if( string( d.c_str() ).compare( "ClearLine" ) == 0 ){
			mClearLine = i;
			continue;
		}
		if( string( d.c_str() ).compare( "GameTime" ) == 0 ){
			mGameTime = i;
			continue;
		}
		if( string( d.c_str() ).compare( "MaxDepth" ) == 0 ){
			mMaxDepth = i;
			continue;
		}
		if( string( d.c_str() ).compare( "MinDepth" ) == 0 ){
			mMinDepth = i;
			continue;
		}
	}
	ifs.close();
}

void DataBase::create(){
	ASSERT( !mInstance );
	mInstance = NEW DataBase();
}

void DataBase::destory(){
	ASSERT( mInstance );
	SAFE_DELETE( mInstance );
}


DataBase* DataBase::instance(){
	return mInstance;
}

int DataBase::clearLine() const {
	return mClearLine;
}

int DataBase::gameTime() const {
	return mGameTime;
}

int DataBase::maxDepth() const {
	return mMaxDepth;
}

int DataBase::minDepth() const {
	return mMinDepth;
}