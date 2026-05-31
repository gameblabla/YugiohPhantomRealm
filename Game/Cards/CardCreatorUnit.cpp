#ifdef USE_GLES
#include <GLES2/gl2.h>
#else
#include <Utility/GLCompat.h> //for GL units
#endif
#include <Game/Cards/CardCreatorUnit.h>
#include <Utility/FileUtil.h>
#include <Game/Cards/Magic/MagicCard.h>
#include <Game/Cards/Magic/ChangeOfHeart.h>
#include <iostream>
#include <string>

namespace Card{

	CardCreatorUnit CardCreatorUnit::cardCreatorUnitInstance;

	CardData CardCreatorUnit::createCard(int cardNo){
		if(cardNo < YUG_LOWEST_CARD_NO || cardNo > YUG_HIGHEST_CARD_NO){
			if(cardNo != YUG_NO_CARD)
				std::cout<<"Card Creator: invalid card number: "<<cardNo<<"\n";
			return blankCard();
		}

		if((337 < cardNo && cardNo < 350)||(777 < cardNo && cardNo < 900)){
			createFileName(cardNo,"GameData/cardData/Magics/");
		}else{
			createFileName(cardNo,"GameData/cardData/Monsters/");
		}
		//std::cout<<fileName.data()<<std::endl;
		std::ifstream input;
		std::string resolvedPath;
		if(!Utility::openInputFile(input, fileName.data(), &resolvedPath)){
			std::cout<<"Card Creator: can't open file with card number: "<<cardNo
				<<" path: "<<fileName.data()<<"\n";
			return blankCard(cardNo);
		}
		//std::cout<<"Card Creator: loading Card\n";
		CardData card;
		//card Number
		untilLBracket(&input); input>>card.cardNumber;
		//card name
		untilLBracket(&input); do{
			card.name.push_back(input.get());
		}while(card.name[card.name.size()-1] != ']');
		card.name[card.name.size()-1] = '\0';
		//render path
		untilLBracket(&input); do{
			card.renderFileName.push_back(input.get());
		}while(card.renderFileName[card.renderFileName.size()-1] != ']');
		card.renderFileName[card.renderFileName.size()-1] = '\0';
		//blurb
		untilLBracket(&input); do{
			card.blurb.push_back(input.get());
		}while(card.blurb[card.blurb.size()-1] != ']');
		card.blurb[card.blurb.size()-1] = '\0';
		untilLBracket(&input); input>>card.monMagTrap;
		untilLBracket(&input); input>>card.attack; card.altAttack = card.attack; card.origAttack = card.attack;
		untilLBracket(&input); input>>card.defense; card.altDefense = card.defense; card.origDefense = card.defense;
		untilLBracket(&input); input>>card.starchips;
		untilLBracket(&input); input>>card.actualType;
		untilLBracket(&input);
		int fType = 0;
		do{
			input>>fType;
			card.fusionTypes.push_back(fType);
		}while(getNextDelim(&input) != ']');
		/*
		untilHash(&input); input>>card.fusionTypes[0];
		untilHash(&input); input>>card.fusionTypes[1];
		untilHash(&input); input>>card.fusionTypes[2];
		*/
		untilLBracket(&input); input>>card.element;
		untilLBracket(&input); input>>card.constellations[0];
		untilLBracket(&input); input>>card.constellations[1];
		card.fieldlessAttack = card.attack;
		card.fieldlessDefense = card.defense;
		card.faceUp = true;
		card.hasAttacked = false;
		card.currentConstellation = YUG_CARD_NO_CURRENT_CON;
		card.pictureTBO = YUG_UNBIND;
		return card;
	}

	void CardCreatorUnit::untilLBracket(std::ifstream* input){
		char ch;
		while(true){
			ch = input->get();
			if(ch == '[')
				return;
		}

	}
	char CardCreatorUnit::getNextDelim(std::ifstream* input){
		char ch;
		while(true){
			ch = input->get();
			if(ch == '[')
				return ch;
			if( ch == ']')
				return ch;
		}

	}

	void CardCreatorUnit::setupParents(CardData* parent){
		parent->bigRender.parentCard = parent;
		parent->smallRender.parentCard = parent;
	}

	namespace{
		void assignCString(std::vector<char>& out, const char* text){
			out.clear();
			if(!text)
				text = "";
			while(*text){
				out.push_back(*text);
				++text;
			}
			out.push_back('\0');
		}
	}

	CardData CardCreatorUnit::blankCard(int cardNo){
		CardData card;
		card.cardNumber = cardNo;
		assignCString(card.name, cardNo == YUG_NO_CARD ? "Empty" : "Missing Card");
		assignCString(card.renderFileName, "GameData/textures/models/badTexture.png");
		assignCString(card.blurb, cardNo == YUG_NO_CARD ? "Empty card slot." : "Missing card data file.");
		card.monMagTrap = YUG_MONSTER_CARD;
		card.attack = 0;
		card.defense = 0;
		card.altAttack = 0;
		card.altDefense = 0;
		card.origAttack = 0;
		card.origDefense = 0;
		card.fieldlessAttack = 0;
		card.fieldlessDefense = 0;
		card.atkStatBoost = 0;
		card.defStatBoost = 0;
		card.atkStatDrop = 0;
		card.defStatDrop = 0;
		card.starchips = 0;
		card.actualType = YUG_INFORMAL_NONE;
		card.fusionTypes.clear();
		card.element = YUG_EARTH_ELEMENT;
		card.constellations[0] = YUG_SUN;
		card.constellations[1] = YUG_MOON;
		card.currentConstellation = YUG_CARD_NO_CURRENT_CON;
		card.faceUp = true;
		card.attackMode = true;
		card.hasAttacked = false;
		card.isVisible = true;
		card.hidden = false;
		card.pictureTBO = YUG_UNBIND;
		card.chain = YUG_CARD_CH_IDLE;
		card.smallRender.parentCard = YUG_UNBIND;
		card.smallRender.doRender = false;
		card.bigRender.parentCard = YUG_UNBIND;
		card.bigRender.doRender = false;
		return card;
	}

	void CardCreatorUnit::createFileName(int cardNo, const char* prefix){
		//----//std::cout<<"Creating card "<<cardNo<<std::endl;
		fileName = std::vector<char>();
		for(const char* ch = prefix; ch && *ch; ++ch)
			fileName.push_back(*ch);
		
		int hDigit = cardNo / 100;
		int tDigit = ((cardNo/10)%10);
		int uDigit = cardNo%10;
		fileName.push_back(hDigit + '0');//get last digit
		fileName.push_back(tDigit + '0');
		fileName.push_back(uDigit + '0');
		fileName.push_back('.');
		fileName.push_back('t');
		fileName.push_back('x');
		fileName.push_back('t');
		fileName.push_back('\0');
		//std::cout<<"\nCard Creator: created Filename: "<<fileName.data()<<std::endl;
	}
	void CardCreatorUnit::setupStream(std::ifstream* input){
		
	}

}
