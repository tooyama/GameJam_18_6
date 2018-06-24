# include <Siv3D.hpp> // OpenSiv3D v0.2.6

void Main()
{
    Window::Resize(1280, 720);
	Graphics::SetBackground(ColorF(1.0, 0.9, 0.7));
    
    const Font font(50,Typeface::Bold);
    
    const Texture piece_P1(U"image/pawn_red.png");
    const Texture piece_P2(U"image/pawn_blue.png");
    
    const Audio systemSE(Wave(GMInstrument::TaikoDrum, PianoKey::C4, 1s));
    const Audio selectSE(Wave(GMInstrument::Woodblock, PianoKey::C4, 1s));
    const Audio pieceSE(Wave(GMInstrument::Woodblock, PianoKey::C2, 1s));
    const Audio getSE(Wave(GMInstrument::Gunshot, PianoKey::C4, 1s));
    
    const double size = 70;
    const Vec2 range(320,50);
    
    const int maxMoveCount = 10;
    const Vec2 moveCountCenter_P1(150,500);
    const Vec2 moveCountCenter_P2(1120,500);
    
    const Vec2 gotPieceRange_P1(50,100);
    const Vec2 gotPieceRange_P2(1020,100);
    const int maxGotPieceCount = 5;
    const Array<Vec2> gotPiecesPos =
    {
        Vec2(size*0.5,0),
        Vec2(size*0.5+size,0),
        Vec2(0,size),
        Vec2(size,size),
        Vec2(size*2,size),
    };
    
    const double restartCount = 0.5;
    
    Stopwatch countTimer_P1(false);
    Stopwatch countTimer_P2(false);
    
    bool isStart = false;
    bool isActive = false;
    bool isWin_P1 = false;
    bool isWin_P2 = false;
    
    int moveCount_P1 = maxMoveCount;
    int moveCount_P2 = maxMoveCount;
    
    int getPieceCount_P1 = 0;
    int getPieceCount_P2 = 0;
    
    Point selectPos_P1(0,0);
    Point selectPos_P2(8,8);
    
    bool isHold = false;
    Point selectedPos(0,0);
    
    Grid<int> squares =
    {
        {1,1,1,1,1,1,1,1,1},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {-1,-1,-1,-1,-1,-1,-1,-1,-1},
    };
    
	while (System::Update())
	{
        //入力
        if(!isStart && KeySpace.down())
        {
            systemSE.playOneShot();
            isStart = true;
            isActive = true;
            countTimer_P1.start();
            countTimer_P2.start();
        }

        if(isActive)
        {
            if(squares[selectPos_P1.x][selectPos_P1.y]==1)
            {
                if(0 < moveCount_P1)
                {
                    if(KeyD.down() && squares[selectPos_P1.x+1][selectPos_P1.y] == 0 && selectPos_P1.x < squares.width()-1)
                    {
                        squares[selectPos_P1.x][selectPos_P1.y] = 0;
                        squares[selectPos_P1.x+1][selectPos_P1.y] = 1;
                        ++selectPos_P1.x;
                        --moveCount_P1;
                        countTimer_P1.restart();
                        pieceSE.playOneShot();
                    }
                    else if(KeyA.down() && squares[selectPos_P1.x-1][selectPos_P1.y] == 0 && 0 < selectPos_P1.x)
                    {
                        squares[selectPos_P1.x][selectPos_P1.y] = 0;
                        squares[selectPos_P1.x-1][selectPos_P1.y] = 1;
                        --selectPos_P1.x;
                        --moveCount_P1;
                        countTimer_P1.restart();
                        pieceSE.playOneShot();
                    }
                    else if(KeyW.down() && squares[selectPos_P1.x][selectPos_P1.y-1] == 0 && 0 < selectPos_P1.x)
                    {
                        squares[selectPos_P1.x][selectPos_P1.y] = 0;
                        squares[selectPos_P1.x][selectPos_P1.y-1] = 1;
                        --selectPos_P1.y;
                        --moveCount_P1;
                        countTimer_P1.restart();
                        pieceSE.playOneShot();
                    }
                    else if(KeyS.down() && squares[selectPos_P1.x][selectPos_P1.y+1] == 0 && selectPos_P1.y < squares.height()-1)
                    {
                        squares[selectPos_P1.x][selectPos_P1.y] = 0;
                        squares[selectPos_P1.x][selectPos_P1.y+1] = 1;
                        ++selectPos_P1.y;
                        --moveCount_P1;
                        countTimer_P1.restart();
                        pieceSE.playOneShot();
                    }
                }
            }
        
            if(KeyQ.down()){ selectPos_P1 = Point(0,0);selectSE.playOneShot();}
            if(KeyRight.down() && selectPos_P1.x < squares.width()-1){ ++selectPos_P1.x; selectSE.playOneShot();}
            if(KeyLeft.down() && 0 < selectPos_P1.x){ --selectPos_P1.x; selectSE.playOneShot();}
            if(KeyUp.down() && 0 < selectPos_P1.y){ --selectPos_P1.y; selectSE.playOneShot();}
            if(KeyDown.down() && selectPos_P1.y < squares.height()-1){ ++selectPos_P1.y; selectSE.playOneShot();}
            
            for (auto i : step(squares.height()))
            {
                for (auto j : step(squares.width()))
                {
                    if(RectF(range.x+i*size,range.y+j*size,size,size).leftClicked())
                    {
                        if(isHold)
                        {
                            if(squares[i][j]==0)
                            {
                                if(i==selectedPos.x && abs(int(j-selectedPos.y)) <= moveCount_P2)
                                {
                                    moveCount_P2 -= abs(int(j-selectedPos.y))+1;
                                    selectPos_P2 = Point(i,j);
                                    squares[i][j] = -1;
                                    squares[selectedPos.x][selectedPos.y] = 0;
                                    isHold = false;
                                    countTimer_P2.restart();
                                    pieceSE.playOneShot();
                                }
                                else if(j==selectedPos.y && abs(int(i-selectedPos.x)) <= moveCount_P2)
                                {
                                    moveCount_P2 -= abs(int(i-selectedPos.x))+1;
                                    selectPos_P2 = Point(i,j);
                                    squares[i][j] = -1;
                                    squares[selectedPos.x][selectedPos.y] = 0;
                                    isHold = false;
                                    countTimer_P2.restart();
                                    pieceSE.playOneShot();
                                }
                            }
                        }
                        else
                        {
                            if(squares[i][j]==-1)
                            {
                                selectPos_P2 = Point(i,j);
                                selectedPos = Point(i,j);
                                isHold = true;
                                selectSE.playOneShot();
                            }
                        }
                    }
                }
            }
            //処理
            for (auto i : step(squares.height()))
            {
                for (auto j : step(squares.width()))
                {
                    if(squares[i][j] == 1)
                    {
                        if(0<i && squares[i-1][j] == -1)
                        {
                            int isGet = 0;
                            for(int k=0; k+i<squares.width(); ++k)
                            {
                                if(squares[k+i][j] == 0) break;
                                if(squares[k+i][j] == -1)
                                {
                                    isGet = k+i;
                                    break;
                                }
                            }
                            
                            for(int k=i; k<isGet; ++k)
                            {
                                squares[k][j] = 0;
                                ++getPieceCount_P2;
                                getSE.playOneShot();
                            }
                        }
                        
                        if(0<j && squares[i][j-1] == -1)
                        {
                            int isGet = 0;
                            for(int k=0; k+j<squares.height(); ++k)
                            {
                                if(squares[i][j+k] == 0) break;
                                if(squares[i][j+k] == -1)
                                {
                                    isGet = k+j;
                                    break;
                                }
                            }
                            
                            for(int k=j; k<isGet; ++k)
                            {
                                squares[i][k] = 0;
                                ++getPieceCount_P2;
                                getSE.playOneShot();
                            }
                        }
                    }
                    else if(squares[i][j] == -1)
                    {
                        if(0<i && squares[i-1][j] == 1)
                        {
                            int isGet = 0;
                            for(int k=0; k+i<squares.width(); ++k)
                            {
                                if(squares[k+i][j] == 0) break;
                                if(squares[k+i][j] == 1)
                                {
                                    isGet = k+i;
                                    break;
                                }
                            }
                            
                            for(int k=i; k<isGet; ++k)
                            {
                                squares[k][j] = 0;
                                ++getPieceCount_P1;
                                getSE.playOneShot();
                            }
                        }
                        
                        if(0<j && squares[i][j-1] == 1)
                        {
                            int isGet = 0;
                            for(int k=0; k+j<squares.height(); ++k)
                            {
                                if(squares[i][j+k] == 0) break;
                                if(squares[i][j+k] == 1)
                                {
                                    isGet = k+j;
                                    break;
                                }
                            }
                            
                            for(int k=j; k<isGet; ++k)
                            {
                                squares[i][k] = 0;
                                ++getPieceCount_P1;
                                getSE.playOneShot();
                            }
                        }
                    }
                }
            }
            
            if(restartCount <= countTimer_P1.sF() && moveCount_P1 < maxMoveCount)
            {
                ++moveCount_P1;
                countTimer_P1.restart();
            }
            
            if(restartCount <= countTimer_P2.sF() && moveCount_P2 < maxMoveCount)
            {
                ++moveCount_P2;
                countTimer_P2.restart();
            }
            
            if(maxGotPieceCount <= getPieceCount_P1 && !isWin_P2)
            {
                systemSE.playOneShot();
                isActive = false;
                isWin_P1 = true;
            }
            
            if(maxGotPieceCount <= getPieceCount_P2 && !isWin_P2)
            {
                systemSE.playOneShot();
                isActive = false;
                isWin_P2 = true;
            }
        }
        //描画
        RectF(range.x,range.y,size*squares.height(), size*squares.width()).drawFrame(0,size*0.1,Palette::Gray);
        
        for (const auto i : step(squares.height()))
        {
            for (const auto j : step(squares.width()))
            {
                RectF(range.x+i*size,range.y+j*size,size,size).drawFrame(size*0.1,0,Palette::Gray);
                
                if(squares[i][j]==1)
                {
                    piece_P1.resized(size).rotated(90_deg).draw(range.x+i*size,range.y+j*size);
                }
                else if(squares[i][j]==-1)
                {
                    piece_P2.resized(size).rotated(-90_deg).draw(range.x+i*size,range.y+j*size);
                }
            }
        }
        
        RectF(range.x+selectPos_P1.x*size,range.y+selectPos_P1.y*size,size,size).drawFrame(size*0.1,size*0.1,ColorF(0.8,0,0));
        RectF(range.x+selectPos_P2.x*size,range.y+selectPos_P2.y*size,size,size).drawFrame(size*0.1,size*0.1,ColorF(0,0,0.8));
        
        for (const auto i : step(gotPiecesPos.size()))
        {
            if(i < getPieceCount_P2) piece_P1.resized(size).draw(gotPieceRange_P2+gotPiecesPos[i]);
            else piece_P1.resized(size).draw(gotPieceRange_P2+gotPiecesPos[i],AlphaF(0.5));
                
            if(i < getPieceCount_P1)piece_P2.resized(size).draw(gotPieceRange_P1+gotPiecesPos[i]);
            else piece_P2.resized(size).draw(gotPieceRange_P1+gotPiecesPos[i],AlphaF(0.5));
        }
        
        for (const auto i : Range(1, maxMoveCount))
        {
            const Vec2 pos_P1 = OffsetCircular(moveCountCenter_P1, 100, 36_deg * i);
            if(i<=moveCount_P1)Circle(pos_P1, 20).draw(ColorF(0.8,0,0));
            else Circle(pos_P1, 20).draw(ColorF(0.8,0,0,0.5));
            
            const Vec2 pos_P2 = OffsetCircular(moveCountCenter_P2, 100, 36_deg * i);
            if(i<=moveCount_P2)Circle(pos_P2, 20).draw(ColorF(0,0,0.8));
            else Circle(pos_P2, 20).draw(ColorF(0,0,0.8,0.5));
        }
        
        if(isHold)
        {
            piece_P2.resized(size).rotated(-90_deg).drawAt(Cursor::Pos(),AlphaF(0.5));
        }
        
        if(!isActive)
        {
            Rect(390,240,490,240).draw(ColorF(1,1,1,0.8));
            
            if(!isStart)font(U"Spaceキーでスタート").drawAt(Window::Center(),ColorF(0,0,0));
            if(isWin_P1)font(U"1P Win!").drawAt(Window::Center(),ColorF(0.8,0,0));
            if(isWin_P2)font(U"2P Win!").drawAt(Window::Center(),ColorF(0,0,0.8));
        }
	}
}
