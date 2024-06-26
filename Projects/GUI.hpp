#pragma once
#include "General.hpp"

class UIText {
   private:
    Text _textObj;
    string _text;
    Color _color;
    Font _font;
    int _characterSize;
    int _x;
    int _y;

    void UpdateText() {
        _textObj.setString(_text);
        _textObj.setPosition(Vector2f(_x, _y));
        _textObj.setFont(_font);
        _textObj.setFillColor(_color);
        _textObj.setCharacterSize(_characterSize);
        _textObj.setOrigin(_textObj.getLocalBounds().left + _textObj.getLocalBounds().width / 2, _textObj.getLocalBounds().top + _textObj.getLocalBounds().height / 2);
    }

   public:
    UIText(int x, int y, int characterSize, string text, Color color) {
        _x = x;
        _y = y;
        _characterSize = characterSize;
        _text = text;
        _color = color;
        if (!_font.loadFromFile("Projects\\Resources\\Fonts\\PlayfairDisplay-Bold.ttf")) {
            cout << "Failed to open font file" << endl;
        }
    }

    Text returnText() {
        UpdateText();
        return _textObj;
    }
};

class UIButton {
   private:
    RectangleShape _button;
    Text _buttonText;
    Font _font;

    Vector2f returnCenter() {
        return Vector2f(_button.getGlobalBounds().left + _button.getGlobalBounds().width / 2, _button.getGlobalBounds().top + _button.getGlobalBounds().height / 2);
    }

   public:
    UIButton(int x, int y, int height, int width, Color color, string text) {
        _button.setFillColor(color);
        _button.setPosition(Vector2f(x, y));
        _button.setSize(Vector2f(width, height));
        _button.setOutlineColor(Color::White);
        _button.setOutlineThickness(2);
        _button.setOrigin(_button.getLocalBounds().left + _button.getLocalBounds().width / 2, _button.getLocalBounds().top + _button.getLocalBounds().height / 2);
        _buttonText.setFillColor(Color::White);
        _buttonText.setPosition(returnCenter());
        _buttonText.setString(text);
        _buttonText.setCharacterSize(20);
        if (!_font.loadFromFile("Projects\\Resources\\Fonts\\PlayfairDisplay-Bold.ttf")) {
            cout << "Failed to open font file" << endl;
        }
        _buttonText.setFont(_font);
        _buttonText.setOrigin(_buttonText.getLocalBounds().left + _buttonText.getLocalBounds().width / 2, _buttonText.getLocalBounds().top + _buttonText.getLocalBounds().height / 2);
    }

    void SetOutlineColor(Color color) {
        _button.setOutlineColor(color);
    }

    void SetFontColor(Color color) {
        _buttonText.setFillColor(color);
    }

    bool IsAt(float x, float y) {
        return _button.getGlobalBounds().contains(x, y);
    }

    RectangleShape GetButton() {
        return _button;
    }

    Text GetButtonText() {
        return _buttonText;
    }
};