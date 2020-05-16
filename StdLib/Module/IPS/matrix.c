/*****************************************************************************
 @Project	: 
 @File 		: BSP.c
 @Details  	: http://www.ccodechamp.com/c-program-to-find-inverse-of-matrix/
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <Common.h>
#include <math.h>
#include "matrix.h"


/*****************************************************************************
 Define
******************************************************************************/
//#define _TRACE_OUT

/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/

/*****************************************************************************
 Local functions
******************************************************************************/
static void matrix_Transpose( double num[2][2], double fac[2][2], double inverse[2][2], int r );
static double matrix_Determinant( double a[2][2], int k );
static void matrix_Cofactor( double num[2][2],double inv[2][2], int f );


/*****************************************************************************
 Implementation
******************************************************************************/
int
MatrixMultiply(
	void 	*pMatA,
	int 	nRowA,
	int 	nColA,
	void 	*pMatB,
	int 	nRowB,
	int 	nColB,
	void 	*pMatMult
	)
{
    int i = 0;
	int j = 0;
	int k = 0;
	
	double (*matA)[2] = pMatA;
	double (*matB)[1] = pMatB;
	double (*matM)[1] = pMatMult;
	
	ASSERT( 0 != pMatA );
	ASSERT( 0 != pMatB );
	ASSERT( 0 != pMatMult );
	
    /* Column of first matrix should be equal to column of second matrix and */
    while( nColA != nRowB )
    {
        printf("Error! column of first matrix not equal to row of second.\n\n");
        printf("Enter rows and column for first matrix: ");
        printf("Enter rows and column for second matrix: ");
    }

    /* Initializing all elements of result matrix to 0 */
    for( i=0; i<nRowA; ++i )
	{
        for( j=0; j<nColB; ++j )
        {
            matM[i][j] = 0;
        }
	}

    /* Multiplying matrices a and b and
     storing result in result matrix */
    for( i=0; i<nRowA; ++i )
	{
        for( j=0; j<nColB; ++j )
		{
            for( k=0; k<nColA; ++k )
            {
                matM[i][j] += matA[i][k]*matB[k][j];
            }
		}
	}

    return 0;
}


BOOL
MatrixInverse(
	double  dMatA[2][2],
	double  dMatInv[2][2],
    int     nOrder
	)
{
    double det = matrix_Determinant( dMatA, nOrder );

    if( 0.0 == det )
    {
        return FALSE;
    }

    matrix_Cofactor( dMatA, dMatInv, nOrder );

    return TRUE;
}


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Local functions
******************************************************************************/
static double
matrix_Determinant( 
    double  dMatA[2][2],
    int     nOrder
    )
{
    /* For calculating Determinant of the Matrix */
    double b[2][2];
    double s = 1.0;
    double det = 0.0;
    int i;
    int j;
    int m;
    int n;
    int c;

    if ( 1 == nOrder )
    {
        return (dMatA[0][0]);
    }
    
    det = 0;
    for( c=0; c<nOrder; c++ )
    {
        m = 0;
        n = 0;
        for( i=0; i<nOrder; i++ )
        {
            for( j=0; j<nOrder; j++ )
            {
                b[i][j] = 0;

                if( (i != 0)&&(j != c) )
                {
                    b[m][n]=dMatA[i][j];
                    if( n<(nOrder-2) )
                    {
                        n++;
                    }
                    else
                    {
                        n = 0;
                        m++;
                    }
                }
            }
        }

        det = det + s * (dMatA[0][c] * matrix_Determinant(b, (nOrder-1)));
        s = -1 * s;
    }
 
    return (det);
}

 
static void
matrix_Cofactor(
    double  dMatA[2][2],
    double  dMatInv[2][2],
    int     nOrder
    )
{
    double b[2][2];
    double fac[2][2];
    int p;
    int q;
    int m;
    int n;
    int i;
    int j;

    for( q=0; q<nOrder; q++ )
    {
        for( p=0; p<nOrder; p++ )
        {
            m = 0;
            n = 0;

            for( i=0; i<nOrder; i++ )
            {
                for( j=0; j<nOrder; j++ )
                {
                    if( (i != q)&&(j != p) )
                    {
                        b[m][n] = dMatA[i][j];

                        if( n<(nOrder-2) )
                        {
                            n++;
                        }
                        else
                        {
                            n = 0;
                            m++;
                        }
                    }
                }
            }

            fac[q][p] = pow( -1, (q+p) ) * matrix_Determinant( b, (nOrder-1) );
        }
    }

    matrix_Transpose( dMatA, fac, dMatInv, nOrder );
}


static void
matrix_Transpose(
    double dMatA[2][2],
    double dMatFac[2][2],
    double inverse[2][2],
    int    nOrder
    )
{
    /* Finding matrix_Transpose of matrix */ 
    int i;
    int j;
    double b[2][2];
    double d;
 
    for( i=0; i<nOrder; i++ )
    {
        for( j=0; j<nOrder; j++ )
        {
            b[i][j] = dMatFac[j][i];
        }
    }

    d = matrix_Determinant( dMatA, nOrder );

    for( i=0; i<nOrder; i++ )
    {
        for( j=0; j<nOrder; j++ )
        {
            inverse[i][j]=b[i][j] / d;
        }
    }
}


/*****************************************************************************
 Interrupt functions
******************************************************************************/























