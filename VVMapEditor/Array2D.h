
#ifndef	_ARRAY2D_H_
#define _ARRAY2D_H_ 

// 2d array on raw 1d memory

template<typename Element> class Array2D
{
	Element*	m_pStart;
	int			m_SizeX, m_SizeY, m_Stride;
public:
	Array2D(Element* pStart, int SizeX, int SizeY, int Stride) : 
		m_pStart(pStart),
		m_SizeX(SizeX), m_SizeY(SizeY), m_Stride(Stride)
		{}

	const Element& operator() (int x, int y) const 
	{
		return m_pStart[y * m_Stride / sizeof(Element) + x];
	}
	Element& operator() (int x, int y) 
	{
		return m_pStart[y * m_Stride / sizeof(Element) + x];
	}
};

template<typename Element> class Array : public std::vector<Element>
{
	int			m_SizeX, m_SizeY;
public:
/*	Array2D( int SizeX, int SizeY, int Stride) : 
		m_pStart(pStart),
		m_SizeX(SizeX), m_SizeY(SizeY), m_Stride(Stride)
		{}*/

	Array( int x = 0, int y = 0) : m_SizeX(x), m_SizeY(y){ resize( x * y );}

	const Element& operator() (int x, int y) const 
	{
		return operator[](y * m_SizeX  + x);
	}
	Element& operator() (int x, int y) 
	{
		return operator[](y * m_SizeX  + x);
	}

	int		GetX() const { return m_SizeX;}
	int		GetY() const { return m_SizeY;}
	void	Resize(int x, int y) 
	{ 
		m_SizeX = x;
		m_SizeY = y;
		resize( m_SizeX * m_SizeY );
	} 
};


#endif