class Player
{
public:
   Player(void);
   Player(char* name, int score);
   ~Player();
protected:
   char* name_;
   int score_;
};

Player::Player(char* name, int score)
{
   score_ = score;
   name_ = name;
}


Player::Player(void) : Player(nullptr,0)
{

}

