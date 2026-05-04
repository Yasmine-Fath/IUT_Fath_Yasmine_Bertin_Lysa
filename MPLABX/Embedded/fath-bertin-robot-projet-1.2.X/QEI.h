#ifndef QEI_H
#define	QEI_H


void InitQEI1();
void InitQEI2();
void QEIUpdateData();
void SendPositionData();

#define DISTROUES 0.218    //distance entre les encodeurs (m)) 

#endif	/* QEI_H */

