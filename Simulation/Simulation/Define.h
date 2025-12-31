#pragma once

// 화면 해상도
#define	WINCX		800
#define	WINCY		600

#define MODE_WIN 1

typedef		D3DXVECTOR2					Vec2;
typedef		D3DXVECTOR3					Vec3;
typedef		D3DXVECTOR4					Vec4;

typedef		D3DXMATRIX						Matrix;

// Singleton
#define NO_COPY(CLASSNAME)								\
			private:										\
			CLASSNAME(const CLASSNAME&) = delete;					\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)					\
			NO_COPY(CLASSNAME)								\
			private:										\
			static CLASSNAME*	m_pInstance;				\
			public:											\
			static CLASSNAME*	GetInstance( void );		\
			static void DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)					\
			CLASSNAME*	CLASSNAME::m_pInstance = NULL;		\
			CLASSNAME*	CLASSNAME::GetInstance( void )	{	\
				if(NULL == m_pInstance) {					\
					m_pInstance = new CLASSNAME;			\
				}											\
				return m_pInstance;							\
			}												\
			void CLASSNAME::DestroyInstance( void ) {		\
				if(NULL != m_pInstance)	{					\
					delete m_pInstance;						\
					m_pInstance = NULL;						\
				}											\
			}