#ifndef GAMESESSION_HPP
#define GAMESESSION_HPP
#include "Session.hpp"
#include "GameType.hpp"
#include <iostream>
#include <sstream>
#include <SFML/Window.hpp>
#include <vector>
#include <unistd.h>
#include <list>
#include "MenuSession.hpp"
// 161 95 + 24 -- > 185 / 119
// afficher selcted in 172 107
// 628 559
// scale 17 23
// 11   12 ecart case debut plateau

#define PLAYER_ONE_TURN 0
#define PLAYER_TWO_TURN 1

#define BACK_TO_MENU    1
#define TRY_AGAIN       2
#define    X_POS        172
#define    Y_POS        106
#define CURSOR_IN_BOARD Input.GetMouseX() > 172 && Input.GetMouseX() < 628 && Input.GetMouseY() > 107  && Input.GetMouseY() < 559
#define NB_LINE         19
#define X_TEXT          100.f
#define Y_TEXT          40.f




class GameSession : public Session
{
public:
    GameSession(GameType *game);
    virtual  ~GameSession();
    virtual Session *update(Gomoku &);
    virtual void draw(Gomoku &);
private:
    GameType    *_gameType;
    sf::Image _gameBoardImage;
    sf::Sprite _gameBoardSprite;
    sf::Image _gameError1Image;
    sf::Sprite _gameError1Sprite;
    sf::Image _gameError2Image;
    sf::Sprite _gameError2Sprite;
    sf::Image _grilleImage;
    sf::Sprite _grilleSprite;
    std::vector<sf::Image> _selectedImage;
    std::vector<sf::Sprite> _selectedSprite;
    sf::Image _whiteImage;
    sf::Sprite _whiteSprite;
    sf::Image _blackImage;
    sf::Sprite _blackSprite;
    sf::Image _cadreImage;
    sf::Sprite _cadreSprite;
    sf::Image _winImage;
    sf::Sprite _winSprite;
    int        _turn;
    int        _notTurn;
    bool       _turndid;
    bool      _endOfGame;
    int       _cursor;
    bool      _cursorIn;
    sf::Clock _Clock;
    float     _timeTeamWhite;
    float     _timeTeamBlack;
    sf::Font    _myFont;
    std::vector<sf::String > _text;

private:
    //          Init methods
    bool        initGameResources();
    bool        initPlayerResources();
    bool        initGraphicalResources();

    //          Display Methods
    void        drawPieces(sf::RenderWindow &App);
    void        showCursor(sf::RenderWindow& App, int);
    void        underlinedTeamTurn(int a, int b);

    //          Game Methods
    void        letsPlay(sf::Event &, const sf::Input &);
    bool        gameTurnOn(const sf::Input &,sf::Event &Event,unsigned int &MouseX, unsigned int &MouseY);
    int         gameTurnOff(Gomoku &,GameType::GameMode,sf::Event &Event,unsigned int &MouseX, unsigned int &MouseY);
    void        iaTurnOff(Gomoku&);
    void        isValidMove(const sf::Input &Input);

    //          Player Methods
    void        changeTurn();
    void        informIA(std::pair<int,int>,std::vector< std::pair<int, int> >  &taken);
    void        sendCapture(std::vector< std::pair<int, int> >  &taken);
    //          Conversion Methods
    std::string        FloatToString(float);
    std::string        IntToString(int);
};

#endif // GAMESESSION_HPP
