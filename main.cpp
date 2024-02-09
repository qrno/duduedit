#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

struct Editor {/*{{{*/
  std::vector<std::wstring> lines{L""};

  enum Mode {
    Normal,
    Insert
  };

  Mode mode = Normal;

  int cur_line = 0;
  int cur_pos = 0;

  void process(sf::Uint32 code) {
    std::cout << cur_line << ':' << cur_pos << " <- " << code << std::endl;
    if (mode == Normal) {
      if (code == 105) {
        mode = Insert;
      } else if (code == 104) { // h
        if (cur_pos-1 >= 0) {
          cur_pos--;
        }
      } else if (code == 106) { // j
        if (cur_line+1 < (int)size(lines)) {
          cur_line++;
          cur_pos = std::min(cur_pos, (int)size(lines[cur_line]));
        }
      } else if (code == 107) { // k
        if (cur_line-1 >= 0) {
          cur_line--;
          cur_pos = std::min(cur_pos, (int)size(lines[cur_line]));
        }
      } else if (code == 108) { // l
        if (cur_pos+1 <= (int)size(lines[cur_line])) {
          cur_pos++;
        }
      } else if (code == 111) { // o
        lines.insert(begin(lines)+cur_line+1, L"");
        cur_line++;
        cur_pos = 0;
        mode = Insert;
      } else if (code == 79) { // O
        lines.insert(begin(lines)+cur_line, L"");
        mode = Insert;
      } else if (code == 68) { // D
        lines.erase(begin(lines)+cur_line);
        if (empty(lines)) lines.push_back(L"");
        if (cur_line >= (int)size(lines)) cur_line--;
        cur_pos = 0;
      } else if (code == 48) { // 0
        cur_pos = 0;
      } else if (code == 65) { // A
        cur_pos = size(lines[cur_line]);
        mode = Insert;
      } else if (code == 103) { // g
        cur_line = 0;
        cur_pos = 0;
      } else if (code == 71) { // G
        cur_line = (int)size(lines)-1;
        cur_pos = 0;
      }
    } else {
      if (code == 8) { // Backspace
        if (cur_pos == 0) {
          if (cur_line > 0) {
            lines[cur_line-1] += lines[cur_line];
            lines.erase(begin(lines)+cur_line, begin(lines)+cur_line+1);
            cur_pos = size(lines[cur_line-1]);
            cur_line--;
          }
          return;
        }
        lines[cur_line].erase(cur_pos-1, 1);
        cur_pos--;
      } else if (code == 13){ // Enter
        lines.insert(begin(lines)+cur_line+1, lines[cur_line].substr(cur_pos));
        lines[cur_line].erase(cur_pos);
        cur_line++;
        cur_pos = 0;
      } else if (code == 27) { // ESC
        mode = Normal;
      } else { // character
        lines[cur_line].insert(cur_pos, sf::String(code));
        cur_pos++;
      }
    }
  }

  std::wstring get_string() {
    std::wstring result;
    for (auto const& line : lines) {
      result += line;
      result += '\n';
    }
    return result;
  }

  int get_index() {
    int total = 0;
    for (int i = 0; i < cur_line; i++)
      total += size(lines[i]) + 1;
    total += cur_pos;
    return total;
  }
};/*}}}*/

sf::Vector2f lerp(sf::Vector2f from, sf::Vector2f to, float c) {
  return from * (1-c) + to * c;
}

int main() {

  sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML Window!");

  Editor E;
  std::wstring show;

  sf::Font font;
  const std::string font_path = "fonts/FiraCode/FiraCode-Regular.ttf";
  if (!font.loadFromFile(font_path)) {
    std::cout << "Couldn't load font from " << font_path << std::endl;
    exit(1);
  }
  int font_size = 72;


  sf::RectangleShape cursor;
  cursor.setFillColor(sf::Color::White);
  const double smooth_movement = 0.2f;

  sf::Text text(E.get_string(), font, font_size);
  text.setFillColor(sf::Color::Cyan);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::TextEntered) {
        sf::Uint32 code = event.text.unicode;
        std::cout << "Received unicode: " << code << std::endl;
        if (E.mode == Editor::Mode::Normal && code == 45) {
          font_size--;
          if (font_size < 20) font_size = 20;
        } else if (E.mode == Editor::Mode::Normal && code == 43){
          font_size++;
          if (font_size > 120) font_size = 120;
        } else {
          E.process(code);
        }
      }
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::Resized) {
        window.setView(sf::View(sf::FloatRect(0,0,event.size.width,event.size.height)));
      }
    }

    window.clear();

    text.setCharacterSize(font_size);
    cursor.setSize(sf::Vector2f(10.f, (double) font_size / 72 * 100));

    auto text_width  = text.getLocalBounds().width;
    auto text_height = text.getLocalBounds().height;

    auto [width, height] = window.getView().getSize();

    sf::Vector2f target_text_pos = {(width-text_width)/2.0f, height*0.5f - text_height * ((float)E.cur_line/size(E.lines))};

    text.setString(E.get_string());
    text.setPosition(lerp(text.getPosition(), target_text_pos, smooth_movement));

    window.draw(text);

    if (E.mode == Editor::Mode::Insert) {
      cursor.setFillColor(sf::Color::Yellow);
    } else {
      cursor.setFillColor(sf::Color::White);
    }

    auto cursor_pos = text.findCharacterPos(E.get_index());
    cursor.setPosition(lerp(cursor.getPosition(), cursor_pos, smooth_movement));

    window.draw(cursor);

    window.display();
  }

}
