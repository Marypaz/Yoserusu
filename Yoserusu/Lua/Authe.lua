function Mode()
	local this = {}

	this.MODE_WAIT = 0;
	this.MODE_NOW = 1;
	this.MODE_END = 2;

	return this;

end

function Authe()
	local this = {}

	this.mAuth = SpriteUtil( "res/image/Auth.png" );
	this.mOK = SpriteUtil( "res/image/Ok.png" );
	this.mRedMaru = SpriteUtil( "res/image/RedMaru.png" );
	this.BlueMaru = SpriteUtil( "res/image/BlueMaru.png" );  
	this.Title_Auth = SpriteUtil( "res/image/Title_Auth.png");

	this.Mode = 0; --EnumMode
	
    this.draw = function( self )
        self.mAuth:draw( 400, 300 );
    end

    return this;

end