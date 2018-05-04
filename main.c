#include <18F4620.h>
#include <stdlib.h>
#fuses HS, NOFCMEN, NOIESO, PUT, NOBROWNOUT, NOWDT
#fuses NOPBADEN, NOMCLR, STVREN, NOLVP, NODEBUG
#use delay(clock=16000000)
#define TX_232        PIN_C6
#define RX_232        PIN_C7
#use RS232(BAUD=9600, XMIT=TX_232, RCV=RX_232, BITS=8,PARITY=N, STOP=1)
#use standard_io(c)
//Variables globales.
//Principales
char aux[5];
char buffer [20];
long operando [3]={0,0,0};
char operador;
long resultado=0;
//Banderas de control
short bufferRXlleno = 0;
short procesoCorrecto = 0;
short mensajePrincipal= 0;
short mensajeError1=0;
short mensajeError2=0;
short mensajeError3=0;
short mensajeResultado=0;
short mensajeErrorMatematicas=0;
short resetError=0;
//Contadores
int contadorComas = 0;
int contadorOperandos = 0;
int controlAux=0;
int contadorBuffer=0;
//Funcion Ingresar Datos---
void ingresaDatos(void)
{
    if(mensajePrincipal == 0)
       {
          printf("El sintaxis correcto de la trama es : >operando1,operando2,operador<");
          printf("\rIngresa los datos del buffer:");
          mensajePrincipal = 1;
       }
    if(kbhit())
       {
          buffer[contadorBuffer]=getch();
          printf("%c",buffer[contadorBuffer]);
          if(contadorBuffer>20 || buffer[contadorBuffer] == '<')
              bufferRXlleno = 1; 
          else
              contadorBuffer += 1;         
       }
}
//---Funcion Verificar Buffer---
void verificarBuffer(void)
{  
    contadorComas=0;
    if(buffer[0]=='>' && buffer[contadorBuffer]=='<' && contadorBuffer>6)
       {
          for(int cont1=1;cont1<contadorBuffer-1;cont1++)
             {
                if(buffer[cont1]==',')
                {
                    contadorComas += 1;
                }
             }  
         if(contadorComas>2)
         {
             if(mensajeError2==0)
             {
              printf("Contenido de buffer incorrecto.\r\n\n\r");
              mensajeError2=1;
              resetError=1;
             }
         }
         else
            {
               for(int cont=1;cont<contadorBuffer-1;cont++)
                  {
                   if(buffer[cont]<'0' || buffer[cont]>'9' || buffer[cont]==',')
                    {
                         if(buffer[cont]==',')
                           {
                              if(contadorOperandos>1)
                                 {
                                    operador=buffer[contadorBuffer-1];
                                    procesoCorrecto = 1;
                                 }
                              else
                              {
                                  
                                  operando[contadorOperandos]=atoi32(aux);
                                  contadorOperandos+=1;
                                  controlAux=0;
                                  aux[0]=0;aux[1]=0;aux[2]=0;aux[3]=0;aux[4]=0;
                              }
                           }
                         else
                            {
                             if(mensajeError3==0)
                                {
                                  printf("Caracteres invalidos.\r\n\n\r");
                                  resetError=1;
                                  mensajeError3=1;
                                }
                            }  
                      }
                     
                    else
                    {
                       aux[controlAux]=buffer[cont];
                       controlAux++;
                    }                         
                  }
            }
       }
    else
    {
      if(mensajeError1==0)
        {
         printf("Trama Incompleta.\n\r\n\r");
         mensajeError1=1;
         resetError=1;
       }
    }
        
}
//---Funcion Matematicas---
void matematicas(void)
{
    switch(operador)
       {
        case '+':
            resultado = (long)operando[0] + (long)operando[1];
        break;
        case '-':
            resultado = (long)operando[0] - (long)operando[1];
        break;
        case '*':
            resultado = (long)operando[0] * (long)operando[1];
        break;
        case'/':
            if(operando[1]==0)
            {
                if(mensajeErrorMatematicas==0)
                {
                    printf("MATH ERROR\n\r\n\r");
                    mensajeErrorMatematicas=1;
                    resetError=1;
                }
            }                
            else
                resultado = (long)operando[0] / (long)operando[1];
        break;
       }
}
//---Funcion Mostrar Resultado---
void mostrarResultado(void)
{
    output_b(resultado);
    output_d(resultado>>8);
    if(mensajeResultado==0)
    {
        printf("\n\r\n\rResultado = %ld\n\r\n\r",resultado);
        mensajeResultado=1;
    }
}
//Funcion Limpiar buffer---
void limpiarBuffer(void)
{
    for(int cont=0; cont<21; cont++)
        buffer[cont]=' ';
    operando[0]=0;
    operando[1]=0;
    operando[2]=0;
    aux[0]=0;
    aux[1]=0;
    aux[2]=0;
    aux[3]=0;
    aux[4]=0;
    resultado=0;
    bufferRXlleno = 0;
    procesoCorrecto = 0;
    contadorBuffer=0;
    mensajePrincipal = 0;
    mensajeError1=0;
    mensajeError2=0;
    mensajeError3=0;
    mensajeResultado=0;
    mensajeErrorMatematicas=0;
    resetError=0;
    contadorComas = 0;
    contadorOperandos = 0;
    controlNumero=0;
    
}
//---Funcion Principal---
void main (void)
{
   setup_oscillator(OSC_16MHZ);
   set_tris_b(0x00);
   set_tris_d(0x00);
   while(1)
   {
       ingresaDatos();
       if(bufferRXlleno == 1)
          {
             verificarBuffer();
             if(resetError==1)
                 limpiarBuffer();
             if(procesoCorrecto==1)
                {
                   matematicas();
                   if(resetError==0)
                        mostrarResultado();
                   limpiarBuffer();
                }
          }
   }
}
