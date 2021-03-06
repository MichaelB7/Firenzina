/*******************************************************************************
Firenzina is a UCI chess playing engine by
Kranium (Norman Schmidt), Yuri Censor (Dmitri Gusev) and ZirconiumX (Matthew Brades).
Rededication: To the memories of Giovanna Tornabuoni and Domenico Ghirlandaio.
Special thanks to: Norman Schmidt, Jose Maria Velasco, Jim Ablett, Jon Dart, Andrey Chilantiev, Quoc Vuong.
Firenzina is a clone of Fire 2.2 xTreme by Kranium (Norman Schmidt). 
Firenzina is a derivative (via Fire) of FireBird by Kranium (Norman Schmidt) 
and Sentinel (Milos Stanisavljevic). Firenzina is based (via Fire and FireBird)
on Ippolit source code: http://ippolit.wikispaces.com/
Ippolit authors: Yakov Petrovich Golyadkin, Igor Igorovich Igoronov,
and Roberto Pescatore 
Ippolit copyright: (C) 2009 Yakov Petrovich Golyadkin
Ippolit date: 92th and 93rd year from Revolution
Ippolit owners: PUBLICDOMAIN (workers)
Ippolit dedication: To Vladimir Ilyich
"This Russian chess ship is a truly glorious achievement of the
 October Revolution and Decembrists movement!"

Firenzina is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Firenzina is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see http://www.gnu.org/licenses/.
*******************************************************************************/

#include "fire.h"

#define BishopKnightMate (Position->Dyn->flags & 128)
#define MaxDist(i,j) (MAX (FileDistance (i, j), RankDistance (i, j)))
#define MinDist(i,j) (MIN (FileDistance (i, j), RankDistance (i, j)))
#define WhiteMinorOnly (Position->Dyn->flags & 32)
#define WhiteHasPiece (Position->Dyn->flags & 2)
#define BlackMinorOnly (Position->Dyn->flags & 64)
#define BlackHasPiece (Position->Dyn->flags & 1)

#define BitBoard3(x, y, z) ((1ULL << (x)) | (1ULL << (y)) | (1ULL << (z)))
#define BitBoard4(w, x, y, z) ((1ULL << (w)) | (1ULL << (x)) | (1ULL << (y)) | (1ULL << (z)))

#define A7A8B8 BitBoard3 (A7, A8, B8)
#define F8G8H8 BitBoard3 (F8, G8, H8)
#define E8F8G8H8 BitBoard4 (E8, F8, G8, H8)
#define H7G8H8 BitBoard3 (H7, G8, H8)
#define A8B8C8D8 BitBoard4 (A8, B8, C8, D8)
#define A8B8C8 BitBoard3 (A8, B8, C8)
#define A1B1A2 BitBoard3 (A1, B1, A1)
#define F1G1H1 BitBoard3 (F1, G1, H1)
#define E1F1G1H1 BitBoard4 (E1, F1, G1, H1)
#define G1H1H2 BitBoard3 (G1, H1, H2)
#define A1B1C1D1 BitBoard4 (A1, B1, C1, D1)
#define A1B1C1 BitBoard3 (A1, B1, C1)
int EvalEnding(typePos* Position, int Value, uint64 wPatt, uint64 bPatt)
    {

    if (BishopKnightMate)
        {
        if (Value > 0)
            {
            if (wBitboardBL)
                Value -= 20 * MIN(MaxDist(A8, Position->bKsq), MaxDist(H1, Position->bKsq))
                   + 10 * MIN(MinDist(A8, Position->bKsq), MinDist(H1, Position->bKsq));
            else
                Value -= 20 * MIN(MaxDist(A1, Position->bKsq), MaxDist(H8, Position->bKsq))
                   + 10 * MIN(MinDist(A1, Position->bKsq), MinDist(H8, Position->bKsq));
            }
        else
            {
            if (bBitboardBL)
                Value += 20 * MIN(MaxDist(A8, Position->wKsq), MaxDist(H1, Position->wKsq))
                   + 10 * MIN(MinDist(A8, Position->wKsq), MinDist(H1, Position->wKsq));
            else
                Value += 20 * MIN(MaxDist(A1, Position->wKsq), MaxDist(H8, Position->wKsq))
                   + 10 * MIN(MinDist(A1, Position->wKsq), MinDist(H8, Position->wKsq));
            }
        }
    if (Position->Dyn->reversible > 50)
        {
        Value *= (114 - Position->Dyn->reversible);
        Value /= 64;
        }
    if (Value > 0)
        {
        if (!Position->wtm && !BlackHasPiece && (bBitboardK ^ bBitboardP) == bBitboardOcc
           && !((bBitboardP >> 8) & ~Position->OccupiedBW) && !(bPatt & wBitboardOcc) && !Position->Dyn->ep
           && !(AttK[Position->bKsq]& ~Position->Dyn->wAtt) && !Position->Dyn->bKcheck)
            Value = 0;
        if (WhiteMinorOnly)
            {
            if (wBitboardN)
                {
                if (wBitboardP == SqSet[A7] && (bBitboardK | AttK[Position->bKsq]) & SqSet[A8])
                    Value = 0;
                if (wBitboardP == SqSet[H7] && (bBitboardK | AttK[Position->bKsq]) & SqSet[H8])
                    Value = 0;
                }
            else if (wBitboardBL && !(wBitboardP & NotH) && (bBitboardK | AttK[Position->bKsq]) & SqSet[H8])
                {
                if (wBitboardP & SqSet[H5] && bBitboardP == (SqSet[G7] | SqSet[H6]))
                    ;
                else
                    Value = 0;
                }
            else if (wBitboardBD && !(wBitboardP & NotA) && (bBitboardK | AttK[Position->bKsq]) & SqSet[A8])
                {
                if (wBitboardP & SqSet[A5] && bBitboardP == (SqSet[B7] | SqSet[A6]))
                    ;
                else
                    Value = 0;
                }
			if (wBitboardBL)
				{
				if (Position->sq[B6] == wEnumP && Position->sq[B7] == bEnumP && !(wBitboardP & ~SqSet[B6])
					&& !(bBitboardP & ~FileB) && !((bBitboardP >> 8) & ~Position->OccupiedBW)
					&& !(wBitboardBL & (wBitboardBL - 1))
				&& (bBitboardK & B8C8D8) && !(wBitboardK &A7A8B8))
					Value = 0;
				if (Position->sq[G6] == wEnumP && Position->sq[G7] == bEnumP && !((wBitboardP | bBitboardP) & ~FileG)
					&& (bBitboardK & E8F8G8H8) && !(wBitboardK &H7G8H8))
					Value = 0;
				}
			if (wBitboardBD)
				{
				if (Position->sq[B6] == wEnumP && Position->sq[B7] == bEnumP && !((wBitboardP | bBitboardP) & ~FileB)
					&& (bBitboardK & A8B8C8D8) && !(wBitboardK &A7A8B8))
					Value = 0;
				if (Position->sq[G6] == wEnumP && Position->sq[G7] == bEnumP && !(wBitboardP & ~SqSet[G6])
					&& !(bBitboardP & ~FileG) && !((bBitboardP >> 8) & ~Position->OccupiedBW)
					&& !(bBitboardBL & (bBitboardBL - 1))
				&& (bBitboardK & F8G8H8) && !(wBitboardK &H7G8H8))
					Value = 0;
				}
            if (!wBitboardP)
                Value = 0;
            }
        }
    else
        {
        if (Position->wtm && !WhiteHasPiece && (wBitboardK ^ wBitboardP) == wBitboardOcc
           && !((wBitboardP << 8) & ~Position->OccupiedBW) && !(wPatt & bBitboardOcc) && !Position->Dyn->ep
           && !(AttK[Position->wKsq]& ~Position->Dyn->bAtt) && !Position->Dyn->wKcheck)
            Value = 0;
        if (BlackMinorOnly)
            {
            if (bBitboardN)
                {
                if (bBitboardP == SqSet[A2] && (wBitboardK | AttK[Position->wKsq]) & SqSet[A1])
                    Value = 0;
                if (bBitboardP == SqSet[H2] && (wBitboardK | AttK[Position->wKsq]) & SqSet[H1])
                    Value = 0;
                }
            else if (bBitboardBD && !(bBitboardP & NotH) && (wBitboardK | AttK[Position->wKsq]) & SqSet[H1])
                {
                if (bBitboardP & SqSet[H4] && wBitboardP == (SqSet[G2] | SqSet[H3]))
                    ;
                else
                    Value = 0;
                }
            else if (bBitboardBL && !(bBitboardP & NotA) && (wBitboardK | AttK[Position->wKsq]) & SqSet[A1])
                {
                if (bBitboardP & SqSet[A4] && wBitboardP == (SqSet[B2] | SqSet[A3]))
                    ;
                else
                    Value = 0;
                }
			if (bBitboardBL)
				{
				if (Position->sq[B3] == bEnumP && Position->sq[B2] == wEnumP && !((bBitboardP | wBitboardP) & ~FileB)
					&& (wBitboardK & A1B1C1D1) && !(bBitboardK &A1B1A2))
					Value = 0;
				if (Position->sq[G3] == bEnumP && Position->sq[G2] == wEnumP && !(bBitboardP & ~SqSet[G3])
					&& !(wBitboardP & ~FileG) && !((wBitboardP << 8) & ~Position->OccupiedBW)
					&& !(bBitboardBL & (bBitboardBL - 1))
				&& (wBitboardK & F1G1H1) && !(bBitboardK &G1H1H2))
					Value = 0;
				}
			if (bBitboardBD)
				{
				if (Position->sq[B3] == bEnumP && Position->sq[B2] == wEnumP && !(bBitboardP & ~SqSet[B3])
					&& !(wBitboardP & ~FileB) && !((wBitboardP << 8) & ~Position->OccupiedBW)
					&& !(bBitboardBD & (bBitboardBD - 1))
				&& (wBitboardK & A1B1C1) && !(bBitboardK &A1B1A2))
					Value = 0;
				if (Position->sq[G3] == bEnumP && Position->sq[G2] == wEnumP && !((bBitboardP | wBitboardP) & ~FileG)
					&& (wBitboardK & E1F1G1H1) && !(bBitboardK &G1H1H2))
					Value = 0;
				}
            if (!bBitboardP)
                Value = 0;
            }
        }
    return Value;
    }
