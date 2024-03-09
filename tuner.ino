int  in[128];
byte NoteV[13]={8,23,40,57,76,96,116,138,162,187,213,241,255};
float f_peaks[5];
const int Mic_pin = A0;
const int MINIMUM_SOUND = 290;
int note;

void setup() {
  Serial.begin(9600);
}

void loop() {
  note = ReadNote();
  switch(note) {
    case 0:
      Serial.println("The Bowl number C");
      break;
    case 2:
      Serial.println("The Bowl number D");
      break;      
    case 4:
      Serial.println("The Bowl number E");
      break;
    case 5:
      Serial.println("The Bowl number F");
      break;
    case 7:
      Serial.println("The Bowl number G");
      break;
    case 9:
      Serial.println("The Bowl number A");
      break;
    case 11:
      Serial.println("The Bowl number B");
      break;
    default:
      Serial.println("Not detected.");
  }
  delay(1000);
}

int ReadNote() {
  long unsigned int a1,b,a2;
  float a;
  float sum1=0,sum2=0;
  float sampling;
  a1 = micros();
        for(int i=0;i<128;i++) {
            a = analogRead(Mic_pin)-500;
            sum1 = sum1 + a;
            sum2 = sum2 + a*a;
            a = a * (sin(i * 3.14 / 128) * sin(i * 3.14 / 128));
            in[i] = 10 * a;
            delayMicroseconds(195);
        }
  b = micros();
  sum1 = sum1 / 128;
  sum2 = sqrt(sum2 / 128);
  sampling = 128000000 / (b - a1);
  if (sum2 - sum1 > 3) {
    FFT(128,sampling);        
    for (int i = 0; i < 12; i++) in[i]=0;
    int j=0,k=0;
      for (int i=0; i<5;i++) {
        if (f_peaks[i] > 1040) f_peaks[i] = 0;
        if (f_peaks[i] >= 65.4   && f_peaks[i]<=130.8) f_peaks[i] = 255 * ((f_peaks[i] / 65.4) - 1);
        if (f_peaks[i] >= 130.8  && f_peaks[i]<=261.6) f_peaks[i] = 255 * ((f_peaks[i] / 130.8) - 1);
        if (f_peaks[i] >= 261.6  && f_peaks[i]<=523.25) f_peaks[i] = 255 * ((f_peaks[i] / 261.6) - 1);
        if (f_peaks[i] >= 523.25 && f_peaks[i]<=1046) f_peaks[i] = 255 * ((f_peaks[i] / 523.25) - 1);
        if (f_peaks[i] >= 1046 && f_peaks[i]<=2093) f_peaks[i] = 255 * ((f_peaks[i] / 1046) - 1);
        if (f_peaks[i] > 255) f_peaks[i] = 254;
        j = 1; k = 0;
         
        while (j == 1) {
          if (f_peaks[i] < NoteV[k]) {
            f_peaks[i] = k;
            j = 0;
          }
          k++;
          if (k > 15) j=0;
        }
        if (f_peaks[i] == 12) f_peaks[i] = 0;
        k = f_peaks[i];
        in[k] = in[k] + (5 - i);
      }
    k = 0; j = 0;
    for (int i = 0; i < 12; i++) {
      if (k < in[i]) {
        k = in[i];
        j = i;
      }
    }

    k = j;

    return k;
  }
}

float FFT(byte N, float Frequency) {
  byte data[8] = {1, 2, 4, 8, 16, 32, 64, 128};
  int a, c1, f, o, x;
  a = N;                          
  for (int i = 0; i < 8; i++) {
    if (data[i] <= a) {
      o = i;
    } 
  }
  o = 7;
  byte in_ps[data[o]]={};
  float out_r[data[o]]={};
  float out_im[data[o]]={};       
  x = 0;
  for (int b = 0; b < o; b++) {
    c1 = data[b];
    f = data[o] / (c1 + c1);
    for (int j = 0; j < c1; j++) { 
      x = x + 1;
      in_ps[x] = in_ps[j] + f;
    }
  }
  for (int i = 0; i < data[o]; i++) {
    if (in_ps[i] < a) {
      out_r[i] = in[in_ps[i]];
    }
    if (in_ps[i] > a) {
      out_r[i] = in[in_ps[i] - a];
    }      
  }

  int i10, i11, n1;
  float e, c, s, tr, ti;
  for (int i = 0; i < o; i++) {
    i10 = data[i];
    i11 = data[o] / data[i + 1];
    e = 6.283 / data[i + 1];
    e = 0 - e;
    n1 = 0;
    for (int j = 0; j < i10; j++) {
      c = cos(e * j);
      s = sin(e * j);
      n1 = j;          
      for (int k = 0; k < i11; k++) {
        tr = c * out_r[i10 + n1] - s * out_im[i10 + n1];
        ti = s * out_r[i10 + n1] + c * out_im[i10 + n1];
        out_r[n1 + i10] = out_r[n1] - tr;
        out_r[n1] = out_r[n1] + tr;
        out_im[n1 + i10] = out_im[n1] - ti;
        out_im[n1] = out_im[n1] + ti;  
        n1 = n1 + i10 + i10;
      }
    }
  }
 
  for (int i = 0; i < data[o - 1]; i++) {
    out_r[i] = sqrt((out_r[i] * out_r[i]) + (out_im[i] * out_im[i]));
    out_im[i] = (i * Frequency) / data[o];
    Serial.print(out_im[i],2); Serial.print("Hz");
    Serial.print("\t");
    Serial.println(out_r[i]); 
  }

  x = 0;
  for (int i = 1; i < data[o - 1] - 1; i++) {
    if (out_r[i] > out_r[i - 1] && out_r[i] > out_r[i + 1]) {
      in_ps[x] = i;
      x = x + 1;
    }    
  }
  s = 0; c = 0;
  for (int i = 0; i < x; i++) {
    for (int j = c; j < x; j++) {
      if (out_r[in_ps[i]] < out_r[in_ps[j]]) {
        s = in_ps[i];
        in_ps[i] = in_ps[j];
        in_ps[j] = s;
      }
    }
    c = c + 1;
  }  
  for (int i = 0; i < 5; i++) {
    f_peaks[i] = (out_im[in_ps[i] - 1] * out_r[in_ps[i] - 1] + out_im[in_ps[i]] * out_r[in_ps[i]] + out_im[in_ps[i] + 1] * out_r[in_ps[i] + 1])
      /
      (out_r[in_ps[i] - 1] + out_r[in_ps[i]] + out_r[in_ps[i] + 1]);
  }
}
