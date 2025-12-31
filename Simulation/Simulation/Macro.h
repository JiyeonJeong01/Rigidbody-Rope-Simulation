#pragma once
#pragma once

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


// Safe Release
template<typename T>
void	Safe_Delete(T& Pointer)
{
	if (nullptr != Pointer)
	{
		Pointer->Release();
		Pointer = nullptr;
	}
}

template<typename T>
void Safe_Release(T& pInstance)
{
	pInstance->Release();
	pInstance = NULL;
}