#ifndef BLOCKITERATOR_H
#define BLOCKITERATOR_H

#include "ilwis.h"
#include "boost/geometry.hpp"

namespace Ilwis {

class BlockIterator;
class GridBlock;

class KERNELSHARED_EXPORT CellIterator : public std::iterator<std::random_access_iterator_tag, double> {
public:
    friend bool operator==(const CellIterator& iter1, const CellIterator& iter2);

    CellIterator(GridBlock *bl, bool end);


    CellIterator& operator=(CellIterator& iter);


    CellIterator& operator++() {
        move(1);
        return *this;
    }
    CellIterator& operator--() {
        move(-1);
        return *this;
    }

    CellIterator operator++(int);
    CellIterator operator--(int);

    double& operator*() ;

   //qint32 position() const;
    Ilwis::Size<> blocksize() const;

private:
    void move(int n);
    GridBlock  *_block;
    qint32 _positionx;
    qint32 _positiony;
    qint32 _positionz;

};

KERNELSHARED_EXPORT bool operator==(const CellIterator& iter1, const CellIterator& iter2);

KERNELSHARED_EXPORT bool operator!=(const CellIterator& iter1, const CellIterator& iter2);

class KERNELSHARED_EXPORT GridBlock {

    friend class BlockIterator;

public:
    enum Pivot{pLEFTUP, pCENTER};

    GridBlock(BlockIterator& biter);
    double& operator()(qint32 x, qint32 y, qint32 z=0);
    double operator()(qint32 x, qint32 y, qint32 z=0) const;
    Size<> size() const;
    CellIterator begin() ;
    CellIterator end() ;
    const BlockIterator& iterator() const;
    bool isValid() const;
    Pixel position() const;
    std::vector<double> toVector(Pivot pivot = pCENTER) const;
private:
    BlockIterator& _iterator;
    std::vector<quint32> _internalBlockNumber;
    std::vector<quint32> _offsets;
    quint32 _blockYSize;
    quint32 _blockXSize;
    quint32 _bandOffset;
    quint64 _XYSize;
    void actualPosition(qint32 &x, qint32 &y, qint32 &z) const;
};

class KERNELSHARED_EXPORT BlockIterator : public PixelIterator {
public:
    friend class GridBlock;

    BlockIterator(IRasterCoverage raster, const Size<> &sz, const BoundingBox& box=BoundingBox(), const Size<> &steps=Size<>());

    GridBlock& operator*() {
        return _block;
    }
    const GridBlock& operator*() const{
        return _block;
    }
    BlockIterator& operator++();
    BlockIterator& operator--();
    BlockIterator end() const ;
    bool operator==(const BlockIterator& iter) const;
    bool operator!=(const BlockIterator& iter) const;
    Size<> blockSize() const;
    void stepsizes(const Size<>& stepsize);
private:
    BlockIterator(quint64 endpos);
    GridBlock _block;
    Size<> _blocksize;
    Size<> _stepsizes;
    double _outside=rILLEGAL;
};


}

#endif // BLOCKITERATOR_H
