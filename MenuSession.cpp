#include <unistd.h>

#include "MenuSession.hpp"
#include "GameSession.hpp"
#include "OptionSession.hpp"
#include "ManualSession.hpp"
#include "SelectModeSession.hpp"
#include "GameType.hpp"
#include "ResourceMgr.hpp"

MenuSession::MenuSession()
{

    if (!_menuImage.LoadFromFile(ResourceMgr::fetch_resource("menu_1.png")))
        std::cout << "Image is missing" << std::endl;

    _menuSprite.SetImage(_menuImage);
    if (!_myFont.LoadFromFile(ResourceMgr::fetch_resource("millenia.ttf"), 50))
        std::cout << "Font is missing" << std::endl;

    if (!_menuOptionImage.LoadFromFile(ResourceMgr::fetch_resource("menu_options.png")))
        std::cout << "Image is missing" << std::endl;
    _menuOptionSprite.SetImage(_menuOptionImage);

    if (!_menuManualImage.LoadFromFile(ResourceMgr::fetch_resource("menu_manual.png")))
        std::cout << "Image is missing" << std::endl;
    _menuManualSprite.SetImage(_menuManualImage);

    if (!_selectModeImage.LoadFromFile(ResourceMgr::fetch_resource("select_mode.png")))
        std::cout << "Image is missing" << std::endl;
    _selectModeSprite.SetImage(_selectModeImage);
    _text.resize(12);

    // Create a graphical string
    _text[0].SetText("New Game");
    _text[1].SetText("Option");
    _text[2].SetText("Manual");
    _text[3].SetText("Exit");
    _text[4].SetText("Back");
    _text[5].SetText("Enable");
    _text[6].SetText("Enable");
    _text[7].SetText("Human Vs Human");
    _text[8].SetText("Human Vs IA");
    _text[9].SetText("IA Vs IA");

    for(int i = 0; i < 10; ++i)
    {
        _text[i].SetFont(_myFont);
        _text[i].SetSize(22);
        _text[i].SetColor(sf::Color(255, 255, 255));
        _text[i].SetPosition(X_IMAGE, Y_IMAGE + ( OFFSET * i ));
    }
    _text[4].SetPosition(600,500);
    _text[5].SetPosition(590,200);
    _text[6].SetPosition(590,330);
    _text[7].SetPosition(300,220);
    _text[8].SetPosition(300,270);
    _text[9].SetPosition(300,320);
    _mode = MENU_MODE;
    _selected = -1;
    _rules.first = false;
    _rules.second = true;

    _firstUp = true;
}

Session    *MenuSession::update(Gomoku &game)
{
    // clean selected Text (to white)
    for(int i = 0; i < 4; ++i)
    {
        _text[i].SetSize(22);
        _text[i].SetColor(sf::Color(255, 255, 255));
    }

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

    const sf::Input& Input =  game.GetApp().GetInput();
    unsigned int XMouse = Input.GetMouseX();
    unsigned int YMouse = Input.GetMouseY();
    (void)Event;
    for(int i = 0; i < 4; ++i)
    {
        underlinedSelectedText(i,XMouse, YMouse);

        if (Event.Type == sf::Event::MouseButtonReleased &&  Event.MouseButton.Button == sf::Mouse::Left)
        {
            if (_selected == SELECT_MODE)
                return new SelectModeSession();
            else if ( _selected == OPTION_MODE)
                return new OptionSession(game.GetRules());
            else if(_selected == MANUAL_MODE)
                return new ManualSession();
            else if (_selected == EXIT_MODE)
                return NULL;
        }
    }
    return this;
}


void    MenuSession::underlinedSelectedText(int &i,unsigned int &MouseX, unsigned int &MouseY)
{
    sf::Vector2f pos =  _text[i].GetPosition();
    if(MouseX >= pos.x && MouseX <= pos.x + X_TEXT && MouseY >= pos.y && MouseY <= pos.y + Y_TEXT)
    {
        _text[i].SetSize(25);
        _text[i].SetColor(sf::Color(255, 255, 0));
        _selected = i;
    }
    else
        _selected = -1;

}

void    MenuSession::draw(Gomoku &game)
{
    drawMenu(game.GetApp());
}

void    MenuSession::drawMenu(sf::RenderWindow &App)
{
    App.Draw(_menuSprite);
    for(int i = 0; i < 4; ++i)
    {
        App.Draw(_text[i]);
    }
}

MenuSession::~MenuSession()
{
}
