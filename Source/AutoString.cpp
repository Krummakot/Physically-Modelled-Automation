
#include "AutoString.hpp"


AutoString::AutoString(){}

AutoString::AutoString(NamedValueSet& parameters, double sampleRate) :
stringLength (*parameters.getVarPointer ("stringLength")),
s0 (*parameters.getVarPointer ("s0")),
s1 (*parameters.getVarPointer ("s1")),
tension (*parameters.getVarPointer ("tension")),
p (*parameters.getVarPointer ("p")),
A (*parameters.getVarPointer ("A")),
E (*parameters.getVarPointer ("E")),
I (*parameters.getVarPointer ("I"))

{
    k = stringLength / sampleRate; // time-step
    c = sqrt(tension/(p*A));
    kappa = sqrt((E*I)/p*A);
    h = sqrt((c * c * k * k + 4.0 * s1 * k
                    + sqrt (pow (c * c * k * k + 4.0 * s1 * k, 2.0)
                            + 16.0 * kappa * kappa * k * k)) * 0.5);
        
    
    N = floor(stringLength / h);                    // Number of gridpoints
    h = stringLength / N;                            // Recalculate gridspacing

    lambdaSq = (c*c*k*k)/(h*h);
    muSq = (kappa*kappa*k*k)/pow(h,4);
    
    uStates.reserve(3);
    
    for(int i = 0; i < 3; ++i) {
        uStates.push_back(std::vector<double>(N+1, 0));
    }
    
    uPtr.resize(3);
    
    for(int i = 0; i < 3; ++i) {
        uPtr[i] = &uStates[i][0];
    }
    
    stringPluckRatio = 0.5;                   // 0 - 1
    
    pluckLoc = truncatePositiveToUnsignedInt(stringPluckRatio*N);
    endNode = N-1;

}

// Destructor
AutoString::~AutoString(){
}

void AutoString::processScheme() {
    for (int l = startNode; l < N-1; ++l) {
        damp = (s0 * k * uPtr[2][l]) + (((2.0 * s1 * k) / (h * h) * (uPtr[1][l+1] - 2.0 * uPtr[1][l] + uPtr[1][l-1] - uPtr[2][l+1] + 2.0 * uPtr[2][l] - uPtr[2][l-1])));
        stiffness = muSq * (uPtr[1][l+2] - 4.0 * uPtr[1][l+1] + 6.0 * uPtr[1][l] - 4.0 * uPtr[1][l-1] + uPtr[1][l-2]);

        uPtr[0][l] = 1.0 / (1.0 + s0 * k) * (2.0 * uPtr[1][l] - uPtr[2][l] + lambdaSq * (uPtr[1][l-1] - 2.0 * uPtr[1][l] + uPtr[1][l+1])) - stiffness + damp;
        
    }
}

void AutoString::updateStates() {
    double* uTmp = uPtr[2];
    uPtr[2] = uPtr[1];
    uPtr[1] = uPtr[0];
    uPtr[0] = uTmp;
}

double AutoString::getOutput(double outPos) {
    return uPtr[0][static_cast<int>(round(N+1) * outPos)];
}

void AutoString::setPluckLoc(double pluckLoc) {
    this->pluckLoc = (round(N+1) * pluckLoc);
}

void AutoString::setOutLoc(double outLoc) {
    this->outPos = outLoc;
}

void AutoString::setDamping(double s0, double s1) {
    this-> s0 = s0;
    this-> s1 = s1;
}

void AutoString::setE(double newE) {
    this-> E = newE;
}

void AutoString::setTension(double newTension) {
    this-> tension = newTension;
}

void AutoString::setStringLength(double newLength) {
    this-> stringLength = newLength;
}

void AutoString::updateCoefficients() {
    c = sqrt(tension/(p*A));
    kappa = sqrt((E*I)/p*A);
    h = sqrt((c * c * k * k + 4.0 * s1 * k
             + sqrt (pow (c * c * k * k + 4.0 * s1 * k, 2.0)
                    + 16.0 * kappa * kappa * k * k)) * 0.5);
    N = floor(stringLength / h);
    h = stringLength / N;
    lambdaSq = (c*c*k*k)/(h*h);
    muSq = (kappa*kappa*k*k)/pow(h,4);

}

void AutoString::excite(int exciteSelection, double velocity, int newWidth) {
    width = newWidth;
    switch (exciteSelection) {
        case 1:
            for (int j = 0; j < width; ++j) {
                double hann = velocity * (1 - cos(2*double_Pi*j/(width-1)));
                uPtr[1][static_cast<int>(j+pluckLoc)] = hann;
                uPtr[2][static_cast<int>(j+pluckLoc)] = hann;
        }; break;
        case 2:
            for(int j = 0; j < width; ++j) {
                if(j <= ((width)/2) && j >= 1) {
                    uPtr[1][static_cast<int>(j+pluckLoc)] = velocity * ((2*j - 1)/(width));
                    uPtr[2][static_cast<int>(j+pluckLoc)] = velocity * ((2*j - 1)/(width));
                } else {
                    uPtr[1][static_cast<int>(j+pluckLoc)] = velocity * (2 - ((2*j - 1)/(width)));
                    uPtr[2][static_cast<int>(j+pluckLoc)] = velocity * (2 - ((2*j - 1)/(width)));
            }
        }; break;
            
        default:
            break;
    }
}
