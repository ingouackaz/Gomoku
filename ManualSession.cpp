#include "ManualSession.hpp"
#include "MenuSession.hpp"
#include "ResourceMgr.hpp"

ManualSession::ManualSession()
{

    if (!_menuManualImage.LoadFromFile(ResourceMgr::fetch_resource("menu_manual.png")))
        std::cout << "Image is missing" << std::endl;
    _menuManualSprite.SetImage(_menuManualImage);
    if (!_myFont.LoadFromFile(ResourceMgr::fetch_resource("millenia.ttf"), 50))
        std::cout << "Font is missing" << std::endl;

    _text.clear();
    _text.resize(1);
    _text[0].SetText("Back");
    _text[0].SetFont(_myFont);
    _text[0].SetSize(22);
    _text[0].SetColor(sf::Color(255, 255, 255));
    _text[0].SetPosition(600,500);

    _selected = -1;

}

Session    *ManualSession::update(Gomoku &game)
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
                return new MenuSession();
        }
    }
    return this;
}

void        ManualSession::draw(Gomoku &game)
{
    game.GetApp().Draw(_menuManualSprite);
    game.GetApp().Draw(_text[0]);
}

ManualSession::~ManualSession()
{

}
