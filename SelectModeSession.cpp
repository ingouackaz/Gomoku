#include "SelectModeSession.hpp"
#include "MenuSession.hpp"
#include "GameSession.hpp"
#include "ResourceMgr.hpp"

SelectModeSession::SelectModeSession()
{
    if (!_selectModeImage.LoadFromFile(ResourceMgr::fetch_resource("select_mode.png")))
        std::cout << "Image is missing" << std::endl;
    _selectModeSprite.SetImage(_selectModeImage);
    if (!_myFont.LoadFromFile(ResourceMgr::fetch_resource("millenia.ttf"), 50))
        std::cout << "Font is missing" << std::endl;

    _text.clear();
    _text.resize(9);

    for(unsigned int i = 0; i < _text.size(); ++i)
    {
        _text[i].SetFont(_myFont);
        _text[i].SetSize(22);
        _text[i].SetColor(sf::Color(255, 255, 255));
    }

    _text[0].SetText("Back");
    _text[1].SetText("Human Vs Human");
    _text[2].SetText("Human Vs AI");
    // try
    _text[3].SetText("Waiting for AI to connect ...");
    _text[4].SetText("Try to communicate");
    // good
    _text[5].SetText("Comunnication successful");
    _text[6].SetText("AI connected succesfull");
    // error

    _text[7].SetText("ERROR : AI not connected");
    _text[8].SetText("ERROR : TimeOut error in comunication");


    _text[0].SetPosition(600,500);
    _text[1].SetPosition(300,270);
    _text[2].SetPosition(300,320);

    // try
    _text[3].SetPosition(250,320);
    _text[4].SetPosition(250,320);
    // good
    _text[5].SetPosition(250,380);
    _text[6].SetPosition(250,380);
    // error
    _text[7].SetPosition(250,420);
    _text[8].SetPosition(250,420);
    _text[7].SetColor(sf::Color(209, 17, 17));
    _text[8].SetColor(sf::Color(209, 17, 17));

    _isModeAi = false;
    _error = 0;
    _aiConnected = false;
}

Session    *SelectModeSession::update(Gomoku &game)
{
    const sf::Input& Input =  game.GetApp().GetInput();
    unsigned int XMouse = Input.GetMouseX();
    unsigned int YMouse = Input.GetMouseY();
    sf::Event Event;

    while (game.GetApp().GetEvent(Event))
    {
        // Close window : exit
        if (Event.Type == sf::Event::Closed)
            return NULL;

        // Escape key : exit
        if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
            return NULL;
    }

    if(_aiConnected == true)
        return new GameSession(new GameType(GameType::P_VS_IA, game.GetRules(), game._ai));
    // SI L'IA EST CONNECTE -> return newGameSession
    if(_isModeAi != true)
    {
        cleanUnselectedText();
        for(unsigned int i = 0; i < _text.size(); ++i)
        {
            underlinedSelectedText(i,XMouse, YMouse);

            if (Event.Type == sf::Event::MouseButtonReleased &&  Event.MouseButton.Button == sf::Mouse::Left)
            {
                if (_selected == BACK_BUTTON)
                    return new MenuSession();
                else if (_selected == HUMAN_VS_HUMAN_BUTTON)
                    return new GameSession(new GameType(GameType::P_VS_P, game.GetRules(),game._ai));
                else if (_selected == HUMAN_VS_IA_BUTTON)
                    _isModeAi = true;
            }
        }
    }
    else
    {
        updateModeAi(game);
    }
    // if Humain vs IA -> try to connect -> Connect or fail / Try to communicate / Communication fail ou launch the game
    return this;
}

void        SelectModeSession::draw(Gomoku &game)
{

    game.GetApp().Draw(_selectModeSprite);
    if (_isModeAi != true)
    {
        if(_error == ERROR_CONNECTION)
            game.GetApp().Draw(_text[7]);
        else if(_error == ERROR_TIMEOUT)
            game.GetApp().Draw(_text[8]);
        for(unsigned int i =1; i < _text.size() - 6; ++i)
        {
            game.GetApp().Draw(_text[i]);
        }
        game.GetApp().Draw(_text[0]);
    }
    else
        drawModeAi(game);
}

void        SelectModeSession::updateModeAi(Gomoku &game)
{
    (void)game;
    //     game.GetApp().Draw(

    /*
    game._ai = bnet_get_ai(4242, false, true, 10000);
    if (!game._ai)
        game.GetApp().Draw(_text[5]);
    else if (!bnet_say_hello(game._ai))
      std::cout << "no hello" << std::endl;
*/
}

void        SelectModeSession::drawModeAi(Gomoku &game)
{
    game.GetApp().Draw(_text[3]);
    game.GetApp().Display();
    game._ai = bnet_get_ai(4242, game.GetRules().first, game.GetRules().second, 3000);
    if (!game._ai)
    {
        _error = ERROR_CONNECTION;
    }
    else
    {
        game.GetApp().Draw(_selectModeSprite);
        game.GetApp().Draw(_text[5]);
        game.GetApp().Draw(_text[4]);
        game.GetApp().Display();
        if (!bnet_say_hello(game._ai))
        {
            _error = ERROR_TIMEOUT;
        }
        else
        {
            // AI CONNECTED
             _aiConnected = true;
             std::cout << "IA CONNECTED" << std::endl;
        }
    }
    _isModeAi = false;
}

SelectModeSession::~SelectModeSession()
{

}
