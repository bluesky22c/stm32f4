/************************************************************
*
* 전체 소스에 걸쳐 사용되는 자료 타입을 재정의 합니다.
*
*************************************************************/
typedef enum
{
	OSC_4MHz=0,
	OSC_8MHz,
	OSC_11MHz,
	OSC_16MHz,
	OSC_MAX
} e_OSC_Freq;

typedef enum
{
	BPS_2400=0,
	BPS_4800,
	BPS_9600,
	BPS_14K,
	BPS_19K,
	BPS_28K,
	BPS_38K,
	BPS_57K,
	BPS_76K,
	BPS_115K,
	BPS_230K,
	BPS_250K,
	BPS_MAX
} e_Bps; 

typedef enum
{
	INTERVAL_100US=0,
	INTERVAL_500US,
	INTERVAL_1MS,
	INTERVAL_MAX
} e_Interval;

typedef struct
{
	unsigned long init_count;
	unsigned long run_count;
	unsigned long tick;
} t_CpuUsage;

typedef enum
{
	INIT_STATE=0,
	DELAY_STATE,
	LED_ON_STATE,
	LED_OFF_STATE,
	LED_FADE_IN_STATE, // PWM 테스트
	LED_FADE_OUT_STATE // PWM 테스트
} e_LedTaskState;

typedef struct
{
	e_LedTaskState task_state;
	unsigned char led_state;
	unsigned char fade_percent; // PWM 테스트
	unsigned long tick;
	unsigned long delay_ms;
} t_LedTask;
