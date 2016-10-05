#include <iostream>
#include <assert.h>
class Shape{
	
public :
	virtual void draw() = 0;
		
};

class Circle : public Shape {
public:
	void draw() {std::cout << "Circle"; }

};

class Rectangle : public Shape{
public:
	void draw() {std::cout << "Rectangle"; }

};
class Color{
	public:
	virtual void paint(Shape *sh) =0; 
};
class RedColor : public Color{
	public:
	void paint(Shape *sh)  {
	std::cout << "Inside paint";
		sh->draw();
	std::cout <<"Red Color\n";}
};

class GreenColor : public Color{
	public:
	void paint(Shape *sh)  {
	sh->draw();
std::cout <<"Green Color\n";}
};


class Object {
	protected:
	Shape *shape;
	public:
	Object() { std::cout << "Object Created";}
	Shape *getShape() {return shape;}
};

class Table : public Object {
public:
	Table() {
		shape = new Rectangle();
		std::cout << "Table Created";
	}
};

class Ball: public Object {
public:
	Ball()  {
		shape = new Circle();
		std::cout << "Ball Created";
	}
};

class GraphicsDriver {
Color *pen;

public :
	GraphicsDriver(Color *_color) {
		pen = _color;
	}
	void drawObject(Object *o) {
		assert(o != NULL);
		assert(pen != NULL);
		std::cout << "Inside drawObject\n";
		pen->paint(o->getShape());
	}

};


int main(void) {

	Object *o = new Ball();
	GraphicsDriver *gd = new GraphicsDriver(new RedColor());
	
	gd->drawObject(o);
	

	return 0;
}
