#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int board[10][10]; //盤面

void user_entry(int user_BW, int *user_entry_error, int *auto_entry);
void cpu_entry(int cpu_BW, int *cpu_entry_error);
void locatable(int locatable_BW, int *locatable_error);
void entry(int entry_x, int entry_y, int entry_BW, int *entry_error);
void result(int *win_count, int *lose_count, int *draw_count, int user_BW);
void print();

int main(){
        
    int i, j, //for文変数
        play_option, //開始と継続と終了
        win_count, lose_count, draw_count, //累計勝敗数
        user_BW, cpu_BW, //○が黒で１、●が白で－１
        user_locatable_error, cpu_locatable_error, //入力チェック
        history_othello[3], //勝敗数履歴
        play_count=0, //プレイ回数
        auto_entry; //自動入力

    FILE *fpR, *fpW;

//乱数初期化
    srand(time(NULL));

//対戦
    while(1){
        while(1){
            play_option=0;
            //開始と終了
            if(play_count==0)printf("開始する場合は１、終了する場合は２\n");
            //継続と終了
            else printf("続ける場合は１、終了する場合は２\n");

            scanf("%d", &play_option);
            if(play_option==1){
                //勝敗データの読み込み
                fpR=fopen("history.othello.csv", "r");
                if(fpR==NULL){
			        printf("ファイル history.othello.csv が開けませんでした。自動的にファイルを作成します。\n");
			        system("touch history.othello.csv");    
                    fpW=fopen("history.othello.csv", "w");
                    fprintf(fpW, "0 0 0");
                    fclose(fpW);
                    fpR=fopen("history.othello.csv", "r");
		        }

                for(i=0;i<3;i++){
                    fscanf(fpR, "%d", &history_othello[i]);
                }

                if(play_count==0){
                    printf("累計%d勝%d敗%d分け\n", history_othello[0], history_othello[1], history_othello[2]);
                    ++play_count;
                }
                win_count=history_othello[0];
                lose_count=history_othello[1];
                draw_count=history_othello[2];
                fclose(fpR);
                
                break;
            }
            else if(play_option==2){   
                return 0;
            }
            else{}
        }
        
        for(i=1;i<9;i++){
            for(j=1;j<9;j++)board[i][j]=0;
        }

        //盤面の初期化
        board[4][4]=board[5][5]=1;
        board[4][5]=board[5][4]=-1;

        //自動入力初期化
        auto_entry=0;

        print();
        //石をランダムで決定
        user_BW=rand()%2;
        //ユーザーが黒の場合
        if(user_BW==1){
            cpu_BW=-1;
            printf("あなたは○(黒)\n");
            while(1){
                user_locatable_error=cpu_locatable_error=0;
                user_entry(user_BW, &user_locatable_error, &auto_entry);
                print();
                cpu_entry(cpu_BW, &cpu_locatable_error);
                if(user_locatable_error==0&&cpu_locatable_error==0)break;
                print();
            }
            
        }
        //ユーザーが白の場合
        else{
            user_BW=-1;
            cpu_BW=1;
            printf("あなたは●(白)\n");
            while(1){
                user_locatable_error=cpu_locatable_error=0;
                cpu_entry(cpu_BW, &cpu_locatable_error);
                print();
                user_entry(user_BW, &user_locatable_error, &auto_entry);
                print();
                if(user_locatable_error==0&&cpu_locatable_error==0)break;
            }
        }

        //結果の表示
        result(&win_count, &lose_count, &draw_count, user_BW);
        printf("累計%d勝%d敗%d分け\n", win_count, lose_count, draw_count);

        //勝敗データの書き込み
        history_othello[0]=win_count;
        history_othello[1]=lose_count;
        history_othello[2]=draw_count;
        fpW=fopen("history.othello.csv", "w");
        for(i=0;i<3;i++){
            fprintf(fpW, "%d ", history_othello[i]);
        }
        fclose(fpW);
    }
}

//ユーザー入力
void user_entry(int user_BW, int *user_locatable_error, int *auto_entry){
    int user_x, user_y, entry_error=0;

    locatable(user_BW, user_locatable_error);
    if(*user_locatable_error==0)return;

    if(*auto_entry==1){
        cpu_entry(user_BW, user_locatable_error);
        return;
    }

    while(1){
        printf("手を入れてください。x=\nまた自動入力にする場合は-1を入力してください");
        scanf("%d", &user_x);
        if(user_x==-1){
            *auto_entry=1;
            cpu_entry(user_BW, user_locatable_error);
            return;
        }
        printf("y=");
        scanf("%d", &user_y);

        if(user_x>0&&user_x<9&&user_y>0&&user_y<9){
            entry(user_x, user_y, user_BW, &entry_error);
            if(entry_error==-1)break;
            else printf("そこには置くことができません\n");
        }
        else printf("そこには置くことができません\n");
    }
}

//cpu入力
void cpu_entry(int cpu_BW, int *cpu_locatable_error){
    int cpu_x, cpu_y, entry_error=0;

    locatable(cpu_BW, cpu_locatable_error);
    if(*cpu_locatable_error==0)return;

    while(1){
        cpu_x=rand()%8+1;
        cpu_y=rand()%8+1;
        entry(cpu_x, cpu_y, cpu_BW, &entry_error);
        if(entry_error==-1)break;
    }
    
}

//入力チェック
void locatable(int locatable_BW, int *locatable_error){
    int i, j, k, l, m, n;

    //すべてのマスで調べる
    for(m=1;m<9;m++){
        for(n=1;n<9;n++){
            if(board[m][n]==0){
                for(i=-1;i<2;i++){
                    for(j=-1;j<2;j++){
                        if(board[m+i][n+j]==-locatable_BW){
                            for(k=2;k<9;k++){
                                //５*５で同色がある場合
                                if(board[m+k*i][n+k*j]==locatable_BW){
                                    *locatable_error=-1;
                                    return;
                                }
                                //５*５で空いている場合
                                else if(board[m+k*i][n+k*j]==0)break;
                                else{}
                            }
                        }
                    }
                }
            }
        }
    }
}

//入力の反映
void entry(int entry_x, int entry_y, int entry_BW, int *entry_error){
    int i, j, k, l;
    if(board[entry_x][entry_y]==0){
        for(i=-1;i<2;i++){
            for(j=-1;j<2;j++){
                //３*３で異色がある場合
                if(board[entry_x+i][entry_y+j]==-entry_BW){
                    for(k=2;k<9;k++){
                        //５*５で同色がある場合
                        if(board[entry_x+k*i][entry_y+k*j]==entry_BW){
                            //間の石を反転
                            for(l=0;l<k;l++)board[entry_x+l*i][entry_y+l*j]=entry_BW;
                            *entry_error=-1;
                            break;
                        }
                        //５*５で空いている場合
                        else if(board[entry_x+k*i][entry_y+k*j]==0)break;
                        else{}
                    }
                }
            }
        }
    }
}

//結果の集計と表示
void result(int *win_count, int *lose_count, int *draw_count, int user_BW){
    int i, j, //for文変数
        cpu_BW=-user_BW,
        player_count=0, cpu_count=0;
        

    for(i=1; i<9; i++){
        for(j=1; j<9; j++){
            if(board[i][j]==user_BW)++player_count;
            else if(board[i][j]==cpu_BW)++cpu_count;
            else{}
        }
    }

    if(player_count>cpu_count){
        ++ *win_count;
        printf("You win\n");
    }
    else if(player_count<cpu_count){
        ++ *lose_count;
        printf("You lose\n");
    }
    else {
        ++ *draw_count;
        printf("Draw\n");
    }
}

//画面表示
void print(){
    int i, j;

    for(i=1;i<9;i++){
            if(i==1)printf("   x");
            printf("%d   ",i);
    }
    printf("\n");

    for(i=0;i<17;i++){
        if(i%2==0){
            if(i==0)printf("y ");
            else printf("  ");

            printf(":---:---:---:---:---:---:---:---:");
        }
        else for(j=0;j<9;j++){
            if(j==0)printf("%d ",(i+1)/2);
            printf("|");

            if(j<8){
                switch(board[j+1][(i+1)/2]){
                    case 0: printf("   ");
                            break;
                    case 1: printf(" ○ ");
                            break;
                    case -1: printf(" ● ");
                            break;
                }
            }
        }
        printf("\n");
    }
    return;
}
