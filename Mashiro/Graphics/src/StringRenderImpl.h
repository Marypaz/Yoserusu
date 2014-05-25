#ifndef INCLUDE_MASHIRO_GRAPHICS_STRING_IMPL_RENDER_H_
#define INCLUDE_MASHIRO_GRAPHICS_STRING_IMPL_RENDER_H_

#include "Mashiro/Mashiro.h"
#include "Mashiro/Base/Impl/ReferenceType.h"
#include "Mashiro/Graphics/StringRender.h"

#include <D2D1.h>
#include <DWrite.h>
#include <Windows.h> 
#include <tchar.h> 
#include <vector>
namespace Mashiro{

namespace Graphics {

class StringRender::Impl : public Mashiro::ReferenceType {
public:
	Impl( Font font, int fontSize, IDWriteFactory* write, ID2D1RenderTarget* backBuffer  ) :
	  mTextFormat( 0 ) {
		HRESULT hr;
		switch( font ){
		case FONT_GOSIC:
			hr = create( L"�l�r�S�V�b�N", fontSize, write );
			break;
		case FONT_MEIRIO:
			hr = create( L"���C���I", fontSize, write );
			break;

		case FONT_MYOUTYOU:
			hr = create( L"�l�r �o����", fontSize, write );
			break;
		}

		if( FAILED( hr ) ){
			hr = E_FAIL;
			STRONG_ASSERT( hr != E_FAIL && "�t�H���g�̐����Ɏ��s���܂���" );
		}

		//�u���V����
		backBuffer->CreateSolidColorBrush( D2D1::ColorF( 0, 0, 0 ), &mBursh );
	}
	~Impl(){
		mTextFormat->Release();
		mTextFormat = 0;
		mBursh->Release();
		mBursh = 0;
	}

	HRESULT create( LPCWSTR fontName, int fontSize, IDWriteFactory* write ){
		// �t�H���g�T�C�Y�����蓾�Ȃ��ꍇ�����I��.0f�w��
		if( fontSize <= 0 )
			fontSize = 16;
		// �e�L�X�g�t�H�[�}�b�g�𐶐�����B
		if( FAILED( write->CreateTextFormat( 
			TEXT( fontName ), NULL,
			DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL, static_cast< FLOAT >( fontSize ), L"en-us", &mTextFormat )))
		{
			return E_FAIL;
		}
		return S_OK;
	}

	void draw( wchar_t* text, int x, int y, ID2D1RenderTarget* backBuffer ){
		backBuffer->BeginDraw();

		FLOAT size = mTextFormat->GetFontSize();

		D2D1_RECT_F rect = D2D1::RectF( 
			static_cast< FLOAT >( x ), 
			static_cast< FLOAT >( y ), 
			static_cast< FLOAT >( x ) + size*wcslen(text), 
			static_cast< FLOAT >( y ) + size );

		backBuffer->DrawText(
			text, wcslen( text ), mTextFormat, rect, mBursh );

		backBuffer->EndDraw();
	}

	void draw(  ID2D1RenderTarget* backBuffer, int x = 0, int y = 0 ){

		backBuffer->BeginDraw();

		//����������C�h������ɕϊ�
		int iSize = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED,
					mBuffer.str().c_str(), (int)mBuffer.str().size(), NULL, 0 );

		//���������T�C�Y���v�f���m��
		std::vector< wchar_t > vecWide( iSize );

		//���߂ăT�C�Y���R�s�[
		iSize = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,
					mBuffer.str().c_str(), (int)mBuffer.str().size(),&vecWide[0],iSize);
		
		//wstring�^�ɕϊ�
		std::wstring wstrValue( vecWide.begin(), vecWide.begin() + vecWide.size() );

		//�t�H���g�T�C�Y���擾
		FLOAT size = mTextFormat->GetFontSize();

		//�������`�悷��̈���w��
		D2D1_RECT_F lauyoutRect = D2D1::RectF(
			static_cast< FLOAT >( x ), 
			static_cast< FLOAT >( y ), 
			static_cast< FLOAT >( x ) + size * wstrValue.length(),
			static_cast< FLOAT >( y ) + size );

		//�������`��
		backBuffer->DrawText( wstrValue.c_str(),
			wstrValue.length(), mTextFormat, lauyoutRect, mBursh );

		mBuffer.str( "" );

		backBuffer->EndDraw();


	}
	
	void setColor( float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f ){
		D2D1_COLOR_F c = { r, g, b, a };
		mBursh->SetColor( c );
	}


	//�`��
	//������
	std::ostringstream mBuffer;
	IDWriteTextFormat* mTextFormat;
	ID2D1SolidColorBrush* mBursh;
};

}
}

#endif