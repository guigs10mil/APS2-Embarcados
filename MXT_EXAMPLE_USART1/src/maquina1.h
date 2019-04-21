/*
 * maquina1.h
 *
 * Created: 15/04/2019 17:33:14
 *  Author: arthu
 */ 


#ifndef MAQUINA1_H_
#define MAQUINA1_H_

typedef struct ciclo t_ciclo;

struct ciclo{
	char nome[32];           // nome do ciclo, para ser exibido
	int  enxagueTempo;       // tempo que fica em cada enxague
	int  enxagueQnt;         // quantidade de enxagues
	int  centrifugacaoRPM;   // velocidade da centrifugacao
	int  centrifugacaoTempo; // tempo que centrifuga
	char heavy;              // modo pesado de lavagem
	char bubblesOn;          // smart bubbles on (???)
	t_ciclo *previous;
	t_ciclo *next;
	tImage *icon;
};

t_ciclo c_rapido = {.nome = "Rapido",
	.enxagueTempo = 5,
	.enxagueQnt = 3,
	.centrifugacaoRPM = 900,
	.centrifugacaoTempo = 5,
	.heavy = 0,
	.bubblesOn = 1,
	.icon = &time
};

t_ciclo c_diario = {.nome = "Diario",
	.enxagueTempo = 15,
	.enxagueQnt = 2,
	.centrifugacaoRPM = 1200,
	.centrifugacaoTempo = 8,
	.heavy = 0,
	.bubblesOn = 1,
	.icon = &day
};

t_ciclo c_pesado = {.nome = "Pesado",
	.enxagueTempo = 10,
	.enxagueQnt = 3,
	.centrifugacaoRPM = 1200,
	.centrifugacaoTempo = 10,
	.heavy = 1,
	.bubblesOn = 1,
	.icon = &zanvil
};

t_ciclo c_enxague = {.nome = "Enxague",
	.enxagueTempo = 10,
	.enxagueQnt = 1,
	.centrifugacaoRPM = 0,
	.centrifugacaoTempo = 0,
	.heavy = 0,
	.bubblesOn = 0,
	.icon = &water
};

t_ciclo c_centrifuga = {.nome = "Centrifuga",
	.enxagueTempo = 0,
	.enxagueQnt = 0,
	.centrifugacaoRPM = 1200,
	.centrifugacaoTempo = 10,
	.heavy = 0,
	.bubblesOn = 0,
	.icon = &vortex
};

t_ciclo c_config = {.nome = "Config",
	.enxagueTempo = 0,
	.enxagueQnt = 0,
	.centrifugacaoRPM = 600,
	.centrifugacaoTempo = 0,
	.heavy = 0,
	.bubblesOn = 0,
	.icon = &gear
};


t_ciclo *initMenuOrder();

#endif /* MAQUINA1_H_ */