#include<bits/stdc++.h>
#include<chrono>
using namespace std;
using namespace std::chrono;

#define FASTIO ios_base::sync_with_stdio(false); cin.tie(NULL);

// Global variables for player, current Board, previous Board
char player;
char opponent;
vector<vector<char>> previousBoard(5,vector<char> (5,'0'));
vector<vector<char>> currentBoard(5,vector<char> (5,'0'));
int depth=4;
int completedMoves;
pair<int,int> finalMove(-1,-1);
bool task=false;

// Reading the input File
void readInput()
{
    string s;
    ifstream inputFile("input.txt");
    getline(inputFile,s);
    player = s[0];
    if(int(player)==int('1'))
    {
        opponent='2';
    }
    else
    {
        opponent='1';
    }
    for(int i=0;i<5;i++)
    {
        getline(inputFile,s);
        for(int j=0;j<5;j++)
        {
            previousBoard[i][j]=s[j];
        }    
    }
    for(int i=0;i<5;i++)
    {
        getline(inputFile,s);
        for(int j=0;j<5;j++)
        {
            currentBoard[i][j]=s[j];
        }
    }
    inputFile.close();
}

// check Empty Board
bool emptyBoard(vector<vector<char>> &board)
{
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            if(board[i][j]!='0')
                return false;
        }
    }
    return true;
}

// check if 2 boards are same
bool compareBoards(vector<vector<char>> &board1, vector<vector<char>> &board2)
{
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            if(board1[i][j]!=board2[i][j])
                return false;
        }
    }
    return true;
}

// Returns the indices of neighbours
vector<pair<int,int>> getNeighbours(int i,int j)
{
    vector<pair<int,int>> neighbours;
    vector<pair<int,int>> coord;
    
    coord.push_back(make_pair(0,1));
    coord.push_back(make_pair(1,0));
    coord.push_back(make_pair(0,-1));
    coord.push_back(make_pair(-1,0));

    for(int k=0;k<4;k++)
    {
        int x=i-coord[k].first;
        int y=j-coord[k].second;
        if(x>=0 && x<5 && y>=0 && y<5)
            neighbours.push_back(make_pair(x,y));
    }
    
    return neighbours;

}

//  Returns strong neighbours
vector<pair<int,int>> getStrongNeighbours(vector<vector<char>> &board,int i,int j)
{
    vector<pair<int,int>> neighbours = getNeighbours(i,j);
    vector<pair<int,int>> strongNB;
    for(int k=0;k<neighbours.size();k++)
    {
        if(board[neighbours[k].first][neighbours[k].second] == board[i][j])
        {
            strongNB.push_back(make_pair(neighbours[k].first,neighbours[k].second));
        }
    }
    return strongNB;
}

bool inQueue(queue<pair<int,int>> ar,pair<int,int> target)
{
    while(!ar.empty())
    {
        if(ar.front()==target)
            return true;
        ar.pop();
    }
    return false;
}

// get all the stones of connecting chain
vector<pair<int,int>> getChainComponent(vector<vector<char>> &board,int i,int j)
{
    vector<pair<int,int>> chain;
    vector<vector<bool>> visited(5,vector<bool> (5,false));
    queue<pair<int,int>> q;
    q.push(make_pair(i,j));
    visited[q.front().first][q.front().second]=true;

    while(!q.empty())
    {
        pair<int,int> key = q.front();
        q.pop();
        chain.push_back(key);
        vector<pair<int,int>> temp = getStrongNeighbours(board,key.first,key.second);
        for(int k=0;k<temp.size();k++)
        {
            if(find(chain.begin(),chain.end(),make_pair(temp[k].first,temp[k].second))==chain.end() && !inQueue(q,make_pair(temp[k].first,temp[k].second)))
            {
                q.push(make_pair(temp[k].first,temp[k].second));
                visited[temp[k].first][temp[k].second]=true;
            }
        }

    }
    return chain;
}



// calculates liberty of the position
int getLiberty(vector<vector<char>> board,int i,int j)
{
    if(board[i][j]=='0')
        return 0;
    vector<pair<int,int>> chainComponents = getChainComponent(board,i,j);
    int liberty=0;
    for(int m=0;m<chainComponents.size();m++)
    {
        vector<pair<int,int>> nb = getNeighbours(chainComponents[m].first,chainComponents[m].second);
        for(int n=0;n<nb.size();n++)
        {
            if(board[nb[n].first][nb[n].second]=='0')
                liberty++;
        }
    }

    return liberty;
}


// returns number of black and white stones on the board
pair<int,int> blackWhiteStones(vector<vector<char>> &board)
{
    int black=0,white=0;
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            if(board[i][j]=='1')
                black++;
            else if(board[i][j]=='2')
                white++;
            else
                continue;
        }
    }
    pair<int,int> ans(black,white);
    return ans;
}

// first order liberty
pair<int,int> heurLiberty(vector<vector<char>> &board)
{
    int black_lib=0, white_lib = 0;
    char key;
    for(int i=0;i<5;i++)
    {

        for(int j=0;j<5;j++)
        {
            if(board[i][j]=='0')
            {
                bool b=false;
                bool w=false;
                vector<pair<int,int>> nb = getNeighbours(i,j);
                for(auto it:nb)
                {
                    if(board[it.first][it.second]=='1')
                    {
                        b=true;
                    }
                    else if(board[it.first][it.second]=='2')
                        w=true;
                }
                if(b)
                    black_lib++;
                else if(w)
                    white_lib++;
            }
        }

    }
    pair<int,int> ans;
    ans.first=black_lib;
    ans.second=white_lib;
    return ans;
}

// edgePiece
pair<int,int> onEdgePieces(vector<vector<char>> &board)
{
    int b=0,w=0;
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            if(i==0 || i==4 || j==0 || j==4)
            {
                if(board[i][j]=='1')
                    b++;
                else if(board[i][j]=='2')
                    w++;
            }
        }
    }
    pair<int,int> ans;
    ans.first=b;
    ans.second=w;
    return ans;
}

// returns the score the board
int getHeuristics(vector<vector<char>> &board,char key,int bDead,int wDead)
{
    pair<int,int> stones=blackWhiteStones(board);
    pair<int,int> ne = onEdgePieces(board);
    pair<int,int> lib = heurLiberty(board);

    int blackLib = lib.first;
    int whiteLib = lib.second;

    int whiteEdge = ne.second;
    int blackEdge = ne.first;
    
    int blackNQ1=0,blackNQ2=0,blackNQ3=0;
    int whiteNQ1=0,whiteNQ2=0,whiteNQ3=0;

    
    int xChange[] = {0,0,1,1};
    int yChange[] = {0,1,0,1};


    
    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            int cnt0=0,cnt1=0,cnt2=0;
            for(int k=0;k<4;k++)
            {
                char c = board[i+xChange[k]][j+yChange[k]];
                if(c=='0')
                    cnt0++;
                else if(c=='1')
                    cnt1++;
                else
                    cnt2++;
                
            }
            if(cnt1==1 && cnt0==3)
                blackNQ1++;
            else if(cnt1==3 && cnt0==1)
                blackNQ1++;
            else if(cnt2==1 && cnt0==3)
                whiteNQ1++;
            else if(cnt2==3 && cnt0==1)
                whiteNQ3++;
            else
            {
                if(i+1<5 && j+1<5)
                {
                    if(board[i][j]=='0' && board[i][j+1]=='1' && board[i+1][j]=='1' && board[i+1][j+1]=='0')
                        blackNQ2++;
                    else if(board[i][j]=='1' && board[i][j+1]=='0' && board[i+1][j]=='0' && board[i+1][j+1]=='1')
                        blackNQ2++;
                    else if(board[i][j]=='2' && board[i][j+1]=='0' && board[i+1][j]=='0' && board[i+1][j+1]=='2')
                        whiteNQ2++;
                    else if(board[i][j]=='0' && board[i][j+1]=='2' && board[i+1][j]=='2' && board[i+1][j+1]=='0') 
                        whiteNQ2++;
                }
                   
            }
        }
    }

    double eulerWhite = (whiteNQ1-whiteNQ3+whiteNQ2)/4;
    double eulerBlack = (blackNQ1-blackNQ3+blackNQ2)/4;


    double finalLiberty, onEdge, euler, pieceDiff;
    double deadDiff=0;
    if(key=='1')
    {
        finalLiberty = blackLib-whiteLib;
        onEdge = whiteEdge-blackEdge;
        euler = eulerBlack-eulerWhite;
        pieceDiff = stones.first-stones.second;
        //deadDiff = wDead*10 - bDead*16;
    }
    else
    {
        finalLiberty = whiteLib-blackLib;
        onEdge = blackEdge-whiteEdge;
        euler = eulerWhite-eulerBlack;
        pieceDiff = stones.second-stones.first;
        //deadDiff = bDead*10 - wDead*16;
    }

    
    double score = min(max(finalLiberty,-4.0),4.0)-4.0*euler+12.0*pieceDiff+onEdge;
    if(key=='1')
    {
        score-=2.5;
    }
    else
        score+=2.5;

    return score;
}

// Returns the list of dead elements
vector<pair<int,int>> checkDeadKeys(vector<vector<char>> board, char key)
{

    vector<pair<int,int>> ans;
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            if(board[i][j]==key)
            {
                if(getLiberty(board, i,j)>0)
                {
                    continue;
                }
                else{
                    ans.push_back(make_pair(i,j));
                }
            }
            
        }
    }
    return ans;

}

// delete Dead Stones from the board
vector<vector<char>> deleteKeys(vector<vector<char>> board,vector<pair<int,int>> ar)
{
    for(int i=0;i<ar.size();i++)
    {
        board[ar[i].first][ar[i].second]='0';
    }
    return board;   
}

// check the validity of the move
bool checkViability(vector<vector<char>> prevBoard, vector<vector<char>> curBoard, char key,int i,int j)
{
    if(i<0 || i>4 || j<0 || j>4)
        return false;
    
    char opp;
    if(key=='1')
        opp='2';
    else
        opp='1';

    curBoard[i][j]=key;
    if(getLiberty(curBoard,i,j)>0)
    {
        return true;
    }
    else
    {
        vector<pair<int,int>> deadKeys = checkDeadKeys(curBoard,opp);

        if(deadKeys.size()==0)
            return false;
        
        curBoard = deleteKeys(curBoard,deadKeys);
        if(compareBoards(curBoard,prevBoard))
        {
            return false;
        }
        return true;
    
    }
    
}

bool worthMove(vector<vector<char>> &board,int x,int y,char p)
{
    char key = board[x][y];
    char opp;
    if(key=='1')
        opp='2';
    else
        opp='1';
    int xChange[] = {-1,-1,-1,0,0,1,1,1};
    int yChange[] = {-1,0,1,-1,1,-1,0,1};
    for(int i=0;i<8;i++)
    {
        int a=x+xChange[i];
        int b = y+yChange[i];
        if(a<5 && a>=0 && b<5 && b>=0)
        {
            if(board[a][b]==p)
                return true;
        }
    }
    return false;
}

// get all the valid moves
vector<pair<int,int>> getViableMoves(vector<vector<char>> prevBoard, vector<vector<char>> curBoard, char key,int currentDepth)
{
    vector<pair<int,int>> moves;
    for(int i=0;i<5;i++)
    {
        for(int j=0;j<5;j++)
        {
            if(curBoard[i][j]=='0')
            {
                if(checkViability(prevBoard,curBoard,key,i,j))
                {
                    if(completedMoves+(depth-currentDepth)<=8)
                    {
                        if(worthMove(curBoard,i,j,key))
                        {
                            moves.push_back(make_pair(i,j));
                        }
                        continue;
                    }
                    moves.push_back(make_pair(i,j));
                }
            }
        }
    }
    if(moves.size()>0)
        return moves;
    moves.push_back(make_pair(-1,-1));
    return moves;
}


// Returns number of completed moves
int getCompletedMoves()
{
    int ans=0;
    if(emptyBoard(previousBoard) && emptyBoard(currentBoard))
    {
        ofstream helperFile;
        helperFile.open("helper.txt");
        helperFile<<0;
        helperFile.close();
        ans=0;
        return ans;
    }
    else if(emptyBoard(previousBoard))
    {
        // cout<<"b"<<endl;
        ofstream helperFile;
        helperFile.open("helper.txt");
        helperFile<<1;
        ans=1;
        helperFile.close();
        return ans;    
    }
    ifstream hIP("helper.txt");
    string ss;
    getline(hIP,ss);
    ans = stoi(ss)+2;
    hIP.close();
    ofstream helperFile;
    helperFile.open("helper.txt");
    helperFile<<ans;
    helperFile.close();
    return ans;
}

double minimax(vector<vector<char>> prevBoard, vector<vector<char>> curBoard, int curDepth, char key,double a,double b,bool maxPlayer,int blackDead, int whiteDead)
{
    if(curDepth==0)
    {
        return getHeuristics(curBoard,key,blackDead,whiteDead);
    }

    vector<pair<int,int>> viableMoves = getViableMoves(prevBoard,curBoard,key,curDepth);

    
    if(maxPlayer)
    {
        double best = INT_MIN;
        for(int ii=0;ii<viableMoves.size();ii++)
        {
            int x = viableMoves[ii].first;
            int y = viableMoves[ii].second;
            
            vector<vector<char>> temp(curBoard);
            
            if(x!=-1)
            {
                temp[x][y] = player;
                vector<pair<int,int>> deadKeys = checkDeadKeys(temp, opponent);
                if(key=='1')
                {
                    whiteDead+=deadKeys.size();
                }
                else
                {
                    blackDead+=deadKeys.size();
                }
                temp = deleteKeys(temp,deadKeys);
            }
            
            double val = minimax(curBoard,temp,curDepth-1,opponent,a,b,false,blackDead,whiteDead);
            if(val>best)
            {
                if(curDepth==depth)
                {
                    
                    finalMove.first=x;
                    finalMove.second=y;
                }
                best=val;
            }
            a = max(a,best);
            if(a>=b)
            {
                break;
            }
        }    
        return best;
    }
    else
    {
        double best = INT_MAX;
        for(int ii=0;ii<viableMoves.size();ii++)
        {
            int x = viableMoves[ii].first;
            int y = viableMoves[ii].second;
            vector<vector<char>> temp(curBoard);
            if(x!=-1)
            {
                temp[x][y] = opponent;
                vector<pair<int,int>> deadKeys = checkDeadKeys(temp, player);
                if(key=='1')
                {
                    whiteDead+=deadKeys.size();
                }
                else
                {
                    blackDead+=deadKeys.size();
                }
                temp = deleteKeys(temp,deadKeys);
            }
            
            double val = minimax(curBoard,temp,curDepth-1,player,a,b,true,blackDead,whiteDead);
            
            if(val<best)
            {
                best=val;
            }
            b = min(b,best);
            if(a>=b)
            {
                break;
            }
        }    
        return best;
    }
}

void writeOutput()
{
    ofstream outputFile;
    outputFile.open("output.txt");
    if(finalMove.first==-1 || finalMove.second==-1)
    {
        outputFile<<"PASS";
        outputFile.close();
    }
    else
    {
        outputFile<<finalMove.first<<","<<finalMove.second;
        outputFile.close();
    }
}


void play()
{
    completedMoves=getCompletedMoves();
    
    if(completedMoves==0)
    {
        finalMove.first=2;
        finalMove.second=2;
    }
    else if(completedMoves==1)
    {
        if(currentBoard[2][2]=='0')
        {
            finalMove.first=2;
            finalMove.second=2;
        }
        else
        {
            finalMove.first=2;
            finalMove.second=1;
        }
    }
    else
    {
        if(completedMoves<6)
            depth=4;
        else if(completedMoves<=18)
            depth=6;
        else
            depth=24-completedMoves;
        int curDepth = depth;
        pair<int,int> move;
        double alpha = INT_MIN;
        double beta = INT_MAX;
        double val = minimax(previousBoard,currentBoard,curDepth,player,alpha,beta,true,0,0);
    }
}


int main()
{
    FASTIO;
    readInput();
    play();
    writeOutput();
    return 0;
}