#include <iostream>
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

class GraphicsDriver {
public :
	void drawObject(Shape *s) {
		s->draw();
	}

};

class Object {
	protected:
	Shape *shape;
	public:
	Shape *getShape() {return shape;}
};

class Table : public Object {
public:
	Table() {
		shape = new Rectangle();
	}
};

class Ball: public Object {
public:
	Ball() {
		shape = new Circle();
	}
};

int main(void) {

	Object *o = new Ball();
	GraphicsDriver *gd = new GraphicsDriver();
	
	gd->drawObject(o->getShape());
//	gd->drawObject(new Rectangle);
	

	return 0;
}
