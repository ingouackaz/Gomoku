#include "OptionSession.hpp"
#include "MenuSession.hpp"
#include "ResourceMgr.hpp"

OptionSession::OptionSession(std::pair<bool,bool> rules)
{
    if (!_myFont.LoadFromFile(ResourceMgr::fetch_resource("millenia.ttf"), 50))
        std::cout << "Font is missing" << std::endl;

    if (!_menuOptionImage.LoadFromFile(ResourceMgr::fetch_resource("menu_options.png")))
        std::cout << "Image is missing" << std::endl;
    _menuOptionSprite.SetImage(_menuOptionImage);

    _text.clear();
    _text.resize(3);


    for(unsigned int i = 0; i < _text.size(); ++i)
    {
        _text[i].SetFont(_myFont);
        _text[i].SetSize(22);
        _text[i].SetColor(sf::Color(255, 255, 255));
        _text[i].SetPosition(X_IMAGE, Y_IMAGE + ( OFFSET * i ));
    }

    _text[0].SetText("Back");

    if(rules.first == true)
        _text[1].SetText("Enabled");
    if(rules.first == false)
        _text[1].SetText("Disabled");
    if(rules.second == true)
        _text[2].SetText("Enabled");
    if(rules.second == false)
        _text[2].SetText("Disabled");

    _text[0].SetPosition(600,500);
    _text[1].SetPosition(590,200);
    _text[2].SetPosition(590,330);
}

Session    *OptionSession::update(Gomoku &game)
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

    cleanUnselectedText();
    for(unsigned int i = 0; i < _text.size(); ++i)
    {
        underlinedSelectedText(i,XMouse, YMouse);

        if (Event.Type == sf::Event::MouseButtonReleased &&  Event.MouseButton.Button == sf::Mouse::Left)
        {
            if (_selected == BACK_BUTTON)
                return new MenuSession();
            else if (_selected == DOUBLE_3_BUTTON)
                game.SetDouble3(ActionDouble3(game.GetRules().first));
            else if (_selected == BREAKABLE_5_BUTTON)
                game.Set5Breakable(ActionBreakable5(game.GetRules().second));
            //std::cout << " DOUBLE 3 : " << game.GetRules().first << "  5 breakable : " << game.GetRules().second << std::endl;
        }
    }
    return this;
}


bool        OptionSession::ActionBreakable5(const bool breakable5)
{
    if (breakable5 == true)
    {
        _text[2].SetText("Disabled");
        return false;
    }
    else
    {
        _text[2].SetText("Enabled");
        return true;
    }
}

bool        OptionSession::ActionDouble3(const bool double3)
{
    if (double3 == true)
    {
        _text[1].SetText("Disabled");
        return false;
    }
    else
    {
        _text[1].SetText("Enabled");
        return true;
    }
}

void        OptionSession::draw(Gomoku &game)
{
    game.GetApp().Draw(_menuOptionSprite);
    for(unsigned int i =0; i < _text.size(); ++i)
    {
        game.GetApp().Draw(_text[i]);
    }

}


OptionSession::~OptionSession()
{

}
