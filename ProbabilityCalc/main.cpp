#include <iostream>
#include <fstream>
void process1();
void process2();

using namespace std;

int main()
{
    //char a;
    process1();
    //cin >> a;
    process2();
}

void process1(){
    fstream sram("sram_prb.csv", fstream::out);
    fstream sramb("sram_bit.csv", fstream::out);
    fstream sram1("sram01_prb.csv", fstream::in);
    fstream sram2("sram02_prb.csv", fstream::in);
    fstream sram3("sram03_prb.csv", fstream::in);
    fstream sram4("sram04_prb.csv", fstream::in);
    fstream sram5("sram05_prb.csv", fstream::in);
    fstream sram6("sram06_prb.csv", fstream::in);
    cout << "\nProcessing Data...";

    double p[7];
    for (int i = 0; i < 262144; i++){
        sram1 >> p[1];
        sram2 >> p[2];
        sram3 >> p[3];
        sram4 >> p[4];
        sram5 >> p[5];
        sram6 >> p[6];
        p[0] = (p[1] + p[2] + p[3] + p[4] + p[5] + p[6]) / 6.0;

        for (int k = 1; k < 7; k++)     //Populate cols A-F with sram probabilities
            sram << p[k] << ',';
        sram << p[0] << ',';

        if ((p[1] == (int)p[1]) && (p[2] == (int)p[2]) && (p[3] == (int)p[3]) && (p[4] == (int)p[4]) && (p[5] == (int)p[5]) && (p[6] == (int)p[6]) && (p[0] == 0.5)){
            sramb << 1 << char(10);
            sram  << 1 << ',' << i << char(10);
        }
        else {
            sramb << 0 << char(10);
            sram  << 0 << ',' << 0 << char(10);
        }
    }
    cout << "DONE.";
    sram.close();
    sramb.close();
    sram1.close();
    sram2.close();
    sram3.close();
    sram4.close();
    sram5.close();
    sram6.close();
}

void process2(){
    fstream sramprb("sram_prb.csv", fstream::in);
    fstream srambit("sram_bit.csv", fstream::in);
    fstream srambyte("sram_byte.csv", fstream::out);
    fstream srammask("sram_mask.csv", fstream::out);

    cout << "\nCreating signatures...";
    //srammask << std::hex;
    for (int i = 0; i < 32768; i++){
        int t = 0, bc = 0;
        double cd[9]={0,0,0,0,0,0,0,0,0};
        int cbt[6]={0,0,0,0,0,0};
        char cm[8];
        for (int j = 7; j >=0; j--){
            double s = 0;
            srambit >> s;

            for (int q = 0; q < 8; q++)
                sramprb >> cd[q] >> cm[q];
            sramprb >> cd[8];
            for (int q = 0; q < 8; q++)
                cbt[q] |= ((int(cd[q]) & 0x1) << j);
            bc += (int(s) & 0x01);
            t |= ((int(s) & 0x01) << j) & 0xFF;
        }
        if (t != 0){
            srambyte << i << ',' << t << ',' << bc << char(10);
        }
        if (bc >= 4){
            srammask << i << ',' << t << ',' << bc;
            for (int q = 0; q < 6; q++)
                srammask << ',' << cbt[q];
            for (int q = 0; q < 6; q++)
                srammask << ',' << (cbt[q] & t);
            srammask << char(10);
        }
    }


    sramprb.close();
    srambit.close();
    srambyte.close();
    srammask.close();
    cout << "DONE.\n";
}
