#include "GameSession.hpp"
#include "bnet_server.hpp"
#include "ResourceMgr.hpp"

// 220 , 122 debut du Map fin 575 , 478


GameSession::GameSession(GameType *game) : _gameType(game)
{
    if(initGameResources())
        if(initPlayerResources())
            if(initGraphicalResources())
            {std::cout << "Loading Resources OK" << std::endl;}
            else
            {std::cout << "Error in Graphical Resources" << std::endl;}
        else
        {std::cout << "Error in Player resources" << std::endl;}
    else
    {std::cout << "Error in Game resources" << std::endl;}
    _endOfGame = false;
}

bool        GameSession::initGameResources()
{
    if (!_gameBoardImage.LoadFromFile(ResourceMgr::fetch_resource("plateau_jeu.png")))
    {std::cout << "Image plateau jeu is missing" << std::endl;return false;}
    // Create the sprite
    _gameBoardSprite.SetImage(_gameBoardImage);
    if (!_blackImage.LoadFromFile(ResourceMgr::fetch_resource("black.png")))
    {std::cout << "Image black is missing" << std::endl;return false;}
    // Create the sprite

    _blackSprite.SetImage(_blackImage);
    if (!_cadreImage.LoadFromFile(ResourceMgr::fetch_resource("cadre.png")))
    {std::cout << "Image cadre  is missing" << std::endl;return false;}
    // Create the sprite
    _cadreSprite.SetImage(_cadreImage);
    if (!_grilleImage.LoadFromFile(ResourceMgr::fetch_resource("grille.png")))
    {std::cout << "Image grille is missing" << std::endl;return false;}
    // Create the sprite
    if (!_gameError1Image.LoadFromFile(ResourceMgr::fetch_resource("error_connection.png")))
    {std::cout << "Image plateau jeu is missing" << std::endl;return false;}
    _gameError1Sprite.SetImage(_gameError1Image);
    if (!_gameError2Image.LoadFromFile(ResourceMgr::fetch_resource("error_hello.png")))
    {std::cout << "Image plateau jeu is missing" << std::endl;return false;}
    _gameError2Sprite.SetImage(_gameError2Image);

    _grilleSprite.SetImage(_grilleImage);
    _selectedImage.resize(3);
    _selectedSprite.resize(3);
    return true;
}

bool    GameSession::initPlayerResources()
{

    if (!_selectedImage[0].LoadFromFile(ResourceMgr::fetch_resource("selected.png")))
    {std::cout << "Image grille is missing" << std::endl;return false;}
    // Create the sprite
    if (!_selectedImage[1].LoadFromFile(ResourceMgr::fetch_resource("selected_2.png")))
    {std::cout << "Image grille is missing" << std::endl;return false;}
    if (!_selectedImage[2].LoadFromFile(ResourceMgr::fetch_resource("selected_3.png")))
    {std::cout << "Image grille is missing" << std::endl;return false;}
    if (!_winImage.LoadFromFile(ResourceMgr::fetch_resource("cadre_loose_wiiin.png")))
    {std::cout << "Image grille is missing" << std::endl;return false;}
    // Create the sprite
    _winSprite.SetImage(_winImage);
    _winSprite.SetPosition(110,280);
    _winSprite.SetColor((sf::Color(0, 255, 255, 98)));

    _selectedSprite[0].SetImage(_selectedImage[0]);
    _selectedSprite[1].SetImage(_selectedImage[1]);
    _selectedSprite[2].SetImage(_selectedImage[2]);
    _selectedSprite[2].SetColor(sf::Color(255, 255, 255, 128));


    if (!_whiteImage.LoadFromFile(ResourceMgr::fetch_resource("white.png")))
    {std::cout << "Image grille is missing" << std::endl;return false;}
    // Create the sprite
    _whiteSprite.SetImage(_whiteImage);
    if (!_myFont.LoadFromFile(ResourceMgr::fetch_resource("millenia.ttf"), 50))
    {std::cout << "Image grille is missing" << std::endl;return false;}
    return true;
}

bool    GameSession::initGraphicalResources()
{

    _text.resize(8);
    for(int i = 0; i < 8; ++i)
    {
        _text[i].SetFont(_myFont);
        _text[i].SetSize(20);
        _text[i].SetColor(sf::Color(255, 255, 255));
    }
    // Create a graphical string
    _text[0].SetText("Black Team");
    _text[1].SetText("White Team");
    _text[2].SetText("Capture :");
    _text[3].SetText("Capture :");
    _text[4].SetText("Team Black wins !");
    _text[5].SetText("Team White wins !");
    _text[6].SetText("Back to menu");
    _text[7].SetText("Try again");

    _text[0].SetPosition(25,75);
    _text[1].SetPosition(674, 75);
    _text[2].SetPosition(670,194);
    _text[3].SetPosition(21, 194);
    _text[4].SetPosition(130,295);
    _text[5].SetPosition(130,295);
    _text[6].SetPosition(320,295);
    _text[7].SetPosition(470,295);

    _turn = PLAYER_ONE_TURN; // random (entre 1 et 2) pour determiner quel player a la main
    _notTurn = PLAYER_TWO_TURN;
    _cursor = 0;
    _turndid = false;
    return true;
}
GameSession::~GameSession()
{
}

Session*    GameSession::update(Gomoku &game)
{

    const sf::Input& Input = game.GetApp().GetInput();
    // Gestion des eventes de jeu -> check if fin de partie etc ...

    // Event par defaut (peu importe le mode de jeu )
    sf::Event Event;
    while (game.GetApp().GetEvent(Event))
    {
        // Close window : exit
        if (Event.Type == sf::Event::Closed)
        {
            if(_gameType->getMode() == GameType::P_VS_IA)
                game.disconnectAi(_gameType->getMode());
            return NULL;
        }
        // Escape key : exit
        if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
        {
            if(_gameType->getMode() == GameType::P_VS_IA)
                game.disconnectAi(_gameType->getMode());
            return NULL;
        }
    }

    // Fucking not turn for bebe
    if(_turn == PLAYER_ONE_TURN)
        _notTurn = PLAYER_TWO_TURN;
    else
        _notTurn = PLAYER_ONE_TURN;

    // Positionnement du cursor
    unsigned int MouseX          =  Input.GetMouseX();
    unsigned int MouseY          =  Input.GetMouseY();

    if(_gameType->getMap().wins(_notTurn) == false && _gameType->getMap().wins(_turn) == false && _endOfGame == false)
    {
        gameTurnOn(Input,Event, MouseX, MouseY);
    }
    else
    {
        int x = gameTurnOff(game,_gameType->getMode(),Event,MouseX, MouseY);
        if (x == BACK_TO_MENU)
        {
            return new MenuSession();
        }
        else if (x == TRY_AGAIN)
        {
            return new GameSession(new GameType(GameType::P_VS_P, game.GetRules(),game._ai));
        }
        else
            (void)x;
    }
    return this;
}
bool    GameSession::gameTurnOn(const sf::Input &Input,sf::Event &Event, unsigned int &MouseX, unsigned int &MouseY)
{
    _selectedSprite[0].SetPosition((( Input.GetMouseX() - 172) / 24 )* 24 + 172, ( (Input.GetMouseY() - 106) / 24 ) * 24 + 106 );
    _selectedSprite[1].SetPosition((( Input.GetMouseX() - 172) / 24 )* 24 + 172, ( (Input.GetMouseY() - 106) / 24 ) * 24 + 106 );
    // Verifier en interrogant l'arbitre si le coup vise par le curseur est valide

    if(CURSOR_IN_BOARD)
        _cursorIn = true;
    else
        _cursorIn = false;

    // Verifier qui doit jouer si Humain desactiver cursor
    letsPlay(Event,Input);

    // Update apture
    _text[3].SetText("Capture : " + IntToString(_gameType->getMap().player_stats(0).ntaken));
    _text[2].SetText("Capture : " + IntToString(_gameType->getMap().player_stats(1).ntaken));
    (void)MouseX;
    (void)MouseY;
    return true;
}

int    GameSession::gameTurnOff(Gomoku &game,GameType::GameMode mode,sf::Event &Event,unsigned int &MouseX, unsigned int &MouseY)
{

    iaTurnOff(game);
    int max = 8; // WHITH IA DISPLAY END

    if(mode == GameType::P_VS_IA)
        max = 7;
    for(int i = 6; i < max; ++i)
    {
        _text[i].SetSize(22);
        _text[i].SetColor(sf::Color(255, 255, 255));
    }
    for(int i = 6; i < max; ++i)
    {

        sf::Vector2f pos =  _text[i].GetPosition();
        if(MouseX >= pos.x && MouseX <= pos.x + X_TEXT && MouseY >= pos.y && MouseY <= pos.y + Y_TEXT)
        {
            //std::cout << "BACK TO" << std::endl;
            _text[i].SetSize(25);
            _text[i].SetColor(sf::Color(255, 255, 0));
            if (i == 6 && Event.Type == sf::Event::MouseButtonReleased &&  Event.MouseButton.Button == sf::Mouse::Left)
            {
                // std::cout << "IN NEW GAME" << std::endl;
                return BACK_TO_MENU;
            }
            else if (i == 7 && Event.Type == sf::Event::MouseButtonReleased &&  Event.MouseButton.Button == sf::Mouse::Left)
            {
                return TRY_AGAIN;
            }
        }
    }
    return 0;
}

void    GameSession::iaTurnOff(Gomoku &game )
{
    if(_endOfGame == true)
    {
        game.disconnectAi(_gameType->getMode());
    }
    else
    {

        bnet_end_reason reason = BNET_END_FIVEALIGN;

        if(_gameType->getMap().player_stats(0).ntaken == 10 || _gameType->getMap().player_stats(1).ntaken == 10)
            reason = BNET_END_CAPTURE;
        if(_notTurn == PLAYER_TWO_TURN)
            game.endOfAi(_gameType->getMode(), true, reason);
        else
            game.endOfAi(_gameType->getMode(), false, reason);
    }
}

void    GameSession::draw(Gomoku &game)
{
    game.GetApp().Draw(_gameBoardSprite);
    if (_cursor == 0 || _cursor == 1)
        showCursor(game.GetApp(),_cursor);
    drawPieces( game.GetApp());
    for(int i = 0; i < 4; ++i)
    {
        if (_turn ==  PLAYER_ONE_TURN)
            _text[0].SetColor(sf::Color(255, 255, 0));
        else
            _text[1].SetColor(sf::Color(255, 255, 0));
        game.GetApp().Draw(_text[i]);
    }
    if(_gameType->getMap().wins(_notTurn) == true|| _gameType->getMap().wins(_turn) == true || _endOfGame == true)
    {
        game.GetApp().Draw(_winSprite);
//        if(_turn == PLAYER_ONE_TURN)
         if(_gameType->getMap().wins(0) == true)
            game.GetApp().Draw( _text[4]);
        else
            game.GetApp().Draw( _text[5]);
        if(_gameType->getMode() == GameType::P_VS_P)
            game.GetApp().Draw( _text[7]);
        game.GetApp().Draw( _text[6]);
    }
}


void    GameSession::showCursor(sf::RenderWindow& App, int player)
{
    (void)player; // a utiliser ?
    // std::cout << "show cursor" << std::endl;
    if (_cursorIn == true) // ou si player Humain
        App.Draw(_selectedSprite[_cursor]);
}


void    GameSession::drawPieces(sf::RenderWindow &App)
{
    for(int x = 0; x < NB_LINE; ++x )
    {
        for(int y = 0; y < NB_LINE; ++y )
        {
            int player(_gameType->getMap().player_at(x,y));

            if (player == Map::PLAYER_BLACK)
            {
                _blackSprite.SetPosition(( x * 24) + X_POS , ( y * 24)  + Y_POS);
                App.Draw(_blackSprite);
            }
            else if (player == Map::PLAYER_WHITE)
            {
                _whiteSprite.SetPosition((x * 24) + X_POS, (y * 24) + Y_POS);
                App.Draw(_whiteSprite);
            }
        }
    }
}

void   GameSession::letsPlay(sf::Event &Event, const sf::Input &Input)
{
    Player  *player;

    if(_turn == PLAYER_ONE_TURN)
    {
        underlinedTeamTurn(0,0);
        player = _gameType->getPlayers().first;
    }
    else
    {
        underlinedTeamTurn(1,1);
        player = _gameType->getPlayers().second;
    }

    // si c'est un Humain
    if( player->getType() == 0)
    {
        isValidMove(Input);

        unsigned int MouseX          =  Input.GetMouseX();
        unsigned int MouseY          =  Input.GetMouseY();

        if (Event.Type == sf::Event::MouseButtonReleased && Event.MouseButton.Button == sf::Mouse::Left && CURSOR_IN_BOARD)
        {

            // Capture
            std::vector< std::pair<int, int> >  taken;


            if ( player->play(std::pair<int,int>( ((MouseX  - X_POS) / 24) , (( MouseY - Y_POS ) / 24) ),taken) == true)
            {
                changeTurn();
                if(_gameType->getMode() == GameType::P_VS_IA)
                    informIA(std::pair<int,int>( ((MouseX  - X_POS) / 24) , (( MouseY - Y_POS ) / 24) ),taken);
            }
        }
    }// si c'est un AI
    else if(player->getType() == 1)
    {
        std::vector< std::pair<int, int> >  taken;
        std::pair<int,int> pos;
        if(player->play(pos,taken))
        {
            changeTurn();
        }
        else
        {
            _endOfGame = true;
            // FIN DU JEU LE JOUEUR HUMAIN A GAGNE
        }
        sendCapture(taken);
    }
    (void)Event;
    (void)Input;
}

void    GameSession::changeTurn()
{
    if (_turn == PLAYER_TWO_TURN)
        _turn = PLAYER_ONE_TURN;
    else if(_turn == PLAYER_ONE_TURN)
        _turn = PLAYER_TWO_TURN;
}

void    GameSession::informIA(std::pair<int, int> pos,std::vector< std::pair<int, int> >  &taken)
{
    // Test si capture
    sendCapture(taken);
    bnet_tell_add(_gameType->getAi(), pos);
}

void    GameSession::sendCapture(std::vector<std::pair<int, int> > &taken)
{
    if(_gameType->getMode() == GameType::P_VS_IA)
    {
        if(taken.size() != 0)
        {
            for(unsigned int i = 0; i < taken.size();  i+=2 )
            {
                bnet_tell_remove(_gameType->getAi(), taken[i] , taken[i+1]);
                std::cout << "TAKEN [ " << i << " ] [" << i+1 << "] " << std::endl;
            }
        }
    }
}

void    GameSession::underlinedTeamTurn(int a, int b)
{
    //_text[a].SetColor(sf::Color(255, 255, 0));
    _text[b].SetColor(sf::Color(255, 255, 255));
    (void)a;
}

void    GameSession::isValidMove(const sf::Input &Input)
{
    int x(((Input.GetMouseX() - X_POS) / 24));
    int y(((Input.GetMouseY() - Y_POS) / 24));

    if (_gameType->getMap().in_bounds(x, y) && _gameType->getMap().is_valid_move(_turn, x, y))
        _cursor = 0;
    else
        _cursor = 1;
}

std::string     GameSession::IntToString(int number)
{
    std::stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stre
}

