#include <iostream>
#include <string>
#include <array>
#include <random>

#include <cstdlib> // for rand() and srand()
#include <ctime> // for time()


// Generate a random number between min and max (inclusive)
// Assumes std::srand() has already been called
// Assumes max - min <= RAND_MAX
int getRandomNumber(int min, int max)
{
	static constexpr double fraction{ 1.0 / (RAND_MAX + 1.0) }; // static used for efficiency, so we only calculate this value once
	// evenly distribute the random number across our range
	return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}

class Creature
{
protected:
	std::string m_name;
	char m_symbol;
	int m_health;
	int m_damage;
	int m_gold;

public:

	Creature(const std::string& name , char symbol , int health , int damage , int gold)
		: m_name{name} , m_symbol{symbol} , m_health{health} , m_damage{damage} , m_gold{gold}
	{		}

	const std::string& getName() const { return m_name; }
	char getSymbol() const { return m_symbol; }
	int  getHealth() const { return m_health; }
	int  getDamage() const { return m_damage; }
	int  getGold()   const { return m_gold; }

	void setHealth(int newHealth) { m_health = newHealth; }
	void setDamage(int newDamage) { m_damage = newDamage; }
	void setGold(int newGold)	  { m_gold = newGold; }

	void reduceHealth(int damage)  {	m_health -= damage;	}

	bool isDead()	{	return (m_health <= 0);	}

	void addGold(int gold)	{	m_gold += gold;	}

};

class Player : public Creature
{
private:
	int m_level{ 1 };

public:
	Player(const std::string& name)
		: Creature{name , '@' , 10 , 1 , 0}
	{		}

	int getLevel() const { return m_level; }

	void levelUp()
	{
		++m_level;
		++m_damage;
	}

	bool hasWon() { return (m_level >= 20); }
};

class Monster : public Creature
{
public:
	enum class Type
	{
		dragon	 ,	orc	 ,	 slime	 ,	max_type
	};

private:
	//A static member function allows us to call the function without having an object of the class. That sort of makes sense
	// for this function because it's a generator. It would be weird to need a Monster object to get a Creature
	static const Creature& getDefaultMonster(Type type)
	{
		//we use static so that we only define monsterData once instead of creating it every time we call this function 
		static std::array<Creature, static_cast<size_t> (Type::max_type)> monsterData{
			{	
				{ "dragon", 'D', 20, 4, 100 },
				{ "orc", 'o', 4, 2, 25 },
				{ "slime", 's', 1, 1, 10 }

			}
		};

		//.at() takes type size_t as argument and makes sure that the returned index exists within the array before returning it
		return monsterData.at(static_cast<size_t> (type));
	}

public: 
	Monster(Type type) : Creature{ getDefaultMonster(type) }
	{

	}

	//A static member function allows us to call the function without having an object of the class. That sort of makes sense
	// for this function because it's a generator. It would be weird to need a Monster object to get a Creature

	static Monster getRandomMonster()
	{
		int num{ getRandomNumber(0, static_cast<int>(Type::max_type) - 1) };
		return Monster{ static_cast<Type>(num) };
	}
};

bool ranSuccessfully()
{
	int num{ getRandomNumber(0,1) };
	if (num == 0)
	{
		std::cout << "You failed to flee\n";
		return false;
	}
	else if (num == 1)
	{
		std::cout << "You successfully fled\n";
		return true;
	}
}

bool  monsterAttacks(Player& player , Monster& monster )
{
	std::cout << "the " << monster.getName() << " hit you for " << monster.getDamage() << " damage\n";
	player.setHealth(player.getHealth() - monster.getDamage()) ;	//reducing the player health 

	return (player.isDead());
}

bool playerAttacks(Player& player, Monster& monster)
{
	std::cout << "you hit the " << monster.getName() << " for " << player.getDamage() << " damage\n";
	monster.setHealth(monster.getHealth() - player.getDamage());

	if (monster.isDead())
	{
		std::cout << "you killed the " << monster.getName() << '\n';
		player.levelUp();
		std::cout << "you are now level " << player.getLevel() << '\n';
		std::cout << "you found " << monster.getGold() << " gold\n";
		player.setGold(player.getGold() + monster.getGold());
	
		return true;
	}
	else
	{
		return false;
	}
	
}


void fightMonster(Player& player)
{
	Monster monster{ Monster::getRandomMonster() };
	std::cout << "you encountered a " << monster.getName() << '(' << monster.getSymbol() << ')' << '\n';
	
	do
	{
		std::cout << "(r)un or (f)ight ?" << '\n';
		char playerDecision{};
		std::cin >> playerDecision;
		if (playerDecision == 'r' || playerDecision == 'R')
		{

			if (!ranSuccessfully())
			{
				monsterAttacks(player, monster);
			}
			else
			{
				break;
			}

		}
		else if (playerDecision == 'f' || playerDecision == 'F')
		{
			if (! playerAttacks(player, monster))
			{
				monsterAttacks(player, monster);
			}
		}
		else
		{
			std::cout << "press \"r\" to run  or \"f\" to fight \n";
		}
	} while ((!player.isDead()) && (! monster.isDead()) );
	
	
}



int main()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr))); // set initial seed value to system clock
	std::rand(); // get rid of first result as it may not be very random with most engines 


	std::string playerName{};
	std::cout << "enter your name: ";
	std::cin >> playerName;

	Player player{ playerName };

	std::cout << "Welcome, " << playerName << '\n';
	
	do
	{
		fightMonster(player);
		
		if (player.hasWon())
		{
			std::cout << "CONGARTULATIONS! you won the game\n" << "you have " << player.getGold() << " gold! Enjoy\n";
			break;
		}
		
		else if (player.isDead())
		{
			std::cout << "you died at level " << player.getLevel() << " with " << player.getGold() << " gold.\n";
			std::cout << "too bad you can't take it with you!\n";
			break;
		}
	} while (true);
	



	return 0;
}