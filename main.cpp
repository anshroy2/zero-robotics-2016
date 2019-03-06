float posLarge[3];
float myState[12];
float itemState[12];
bool test;
float attitude[3];
bool notYet;
int timer;
float err;
int start;
int end;
void init(){
    game.dropSPS();
    game.getItemLoc(posLarge, 1); //for both red and blue
    test=false;
    float attitude[]={posLarge[0], posLarge[1], posLarge[2]}; //still doing it here though to start early
    api.setAttitudeTarget(attitude);
    notYet=true;
    timer=-1;
    err=0.06;
}
 
void loop(){
    api.getMyZRState(myState);
    if(!test)
    {
        attitude[0]=posLarge[0]-myState[0];
        attitude[1]=posLarge[1]-myState[1];
        attitude[2]=posLarge[2]-myState[2];
    }
    float vel1to2[] = {-10000,-10000,-9000};
    float vel2To3[] = {700,800,750};
    game.getItemZRState(itemState, 1);
    int timeNow = api.getTime();
    
    api.setAttitudeTarget(attitude);
    if(timeNow<7)
    {
        api.setVelocityTarget(vel1to2);
    }
    if((timeNow>7&&timeNow<14) || (timeNow==7))           //going to first large object
    {
        api.setPositionTarget(posLarge);
    }
    if(timeNow > 15&&(!test) && timeNow < 18)          //slowing down on path to first large object
    {
        float opposite[]={-1*myState[3], -1*myState[4], -1*myState[5]};
        api.setVelocityTarget(opposite);
    }
    if((timeNow > 18 || timeNow==18)&&(!test)) //fine tune position
    {
        posLarge[1]=posLarge[1]+0.160;
        api.setPositionTarget(posLarge);
        posLarge[1]=posLarge[1]-0.160;
    }
    float vect1[]={myState[6],myState[7],myState[8]}; //attitude vector of sattelite
    float vect2[]={itemState[0]-myState[0],itemState[1]-myState[1],itemState[2]-myState[2]};//vector from middle of sattelite to middle of item
    if(game.hasItem(1)==0 && sqrtf(mathSquare(myState[0]-posLarge[0]) + mathSquare(myState[1]-posLarge[1])+ mathSquare((myState[2]-posLarge[2])))<0.173 && sqrtf((mathSquare(myState[3])+mathSquare(myState[4])+mathSquare(myState[5])))<0.011 && isFacing(vect1,vect2))
    {
        game.dockItem(); 
        test=true;
        //game.dropSPS();
        start = timeNow;
        end = timeNow;
    }
    if(game.hasItem(1) == 1&&game.getNumSPSHeld() == 1)
    {
        float velLast[] = {-10000,-10000,-10000};
        api.setVelocityTarget(velLast);
        end++;
    }
    if (timeNow == 21)
    {
        game.dropSPS();
    }
    if (timeNow == 45)
    {
        game.dropSPS();
    }
    //FIRST ITEM IS DOCKED AT 21-22 SECONDS
    if(!(timer==-1))
    {
        timer=timer+1;
    }
    if (timeNow >= 21 && notYet&&game.hasItem(1)==1) 
    {
        float nextBit[] = {-10000, -10000, -10000};
        api.setVelocityTarget(nextBit);
        if(timer==-1)
        {
            timer=0;
        }
    }
    if(timer==8) //timer is how many seconds after. Hardcoded or not doesnt matter really tbh
    {
        notYet=false;
        
        float opposite[]={-1*myState[3], -1*myState[4], -1*myState[5]};
        api.setVelocityTarget(opposite);
        
    }
    if(timer==12)
    {
        game.dropSPS();  //DROPPING SECOND SPS Lets just drop the third SPS when we dock to the object. 
                        //We want to drop all the SPS in under 35 seconds and get atleast one point!
       
        
    }
    if(!notYet) 
    {
        float pos[]={0.52, -0.52, 0.52};
        api.setPositionTarget(pos);
        
        if(fabsf(myState[0]-pos[0])<err&&fabsf(myState[1]-pos[1])<err&&fabsf(myState[2]-pos[2])<err)
        {
            game.dropSPS();
        }
    }
    
    //third SPS should be dropped at negative Y, pos X, pos Z
    
}

bool isFacing(float vect1[3], float vect2[3]) //vect1 is your attitude, vect2 is center of you to center of item
{
    float size=mathVecMagnitude(vect2,3);
    float num=vect1[0]*vect2[0]+vect1[1]*vect2[1]+vect1[2]*vect2[2];
    num=num/size;
    //float den=mathVecMagnitude(vect1,3)*mathVecMagnitude(vect2,3);
    float den=1.0;
    if(fabsf(acosf(num/den))<0.25) //MUST BE 0.25 OR LESSSSSSSSSSSSS :)
    {
        return true;
    }
    return false;
}

