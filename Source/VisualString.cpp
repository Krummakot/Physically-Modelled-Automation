
#include "VisualString.hpp"


VisualString::VisualString(){}

VisualString::VisualString(NamedValueSet& parameters, double sampleRate) :
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
        
    
    N = floor(stringLength / h);
    h = stringLength / N;

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
    
    stringPluckRatio = 0.5;
    
    pluckLoc = truncatePositiveToUnsignedInt(stringPluckRatio*N);
    endNode = N-1;
}

// Destructor
VisualString::~VisualString(){
}

void VisualString::processScheme() {
    for (int l = startNode; l < N-1; ++l) {
        damp = (s0 * k * uPtr[2][l]) + (((2.0 * s1 * k) / (h * h) * (uPtr[1][l+1] - 2.0 * uPtr[1][l] + uPtr[1][l-1] - uPtr[2][l+1] + 2.0 * uPtr[2][l] - uPtr[2][l-1])));
        stiffness = muSq * (uPtr[1][l+2] - 4.0 * uPtr[1][l+1] + 6.0 * uPtr[1][l] - 4.0 * uPtr[1][l-1] + uPtr[1][l-2]);

        uPtr[0][l] = 1.0 / (1.0 + s0 * k) * (2.0 * uPtr[1][l] - uPtr[2][l] + lambdaSq * (uPtr[1][l-1] - 2.0 * uPtr[1][l] + uPtr[1][l+1])) - stiffness + damp;
    }
}

void VisualString::updateStates() {
    double* uTmp = uPtr[2];
    uPtr[2] = uPtr[1];
    uPtr[1] = uPtr[0];
    uPtr[0] = uTmp;
}

double VisualString::getOutput(double outPos) {
    return uPtr[0][static_cast<int>(round(N+1) * outPos)];
}

void VisualString::setPluckLoc(double pluckLoc) {
    this->pluckLoc = (round(N+1) * pluckLoc);
}

void VisualString::setOutLoc(double outLoc) {
    this->outPos = outLoc;
}

void VisualString::setDamping(double s0, double s1) {
    this-> s0 = s1;
    this-> s1 = s1;
}

void VisualString::setE(double newE) {
    this-> E = newE;
}

void VisualString::setTension(double newTension) {
    this-> tension = newTension;
}

void VisualString::setStringLength(double newLength) {
    this-> stringLength = newLength;
}

void VisualString::updateCoefficients() {
    c = sqrt(tension/(p*A));
    h = sqrt((c * c * k * k + 4.0 * s1 * k
             + sqrt (pow (c * c * k * k + 4.0 * s1 * k, 2.0)
                    + 16.0 * kappa * kappa * k * k)) * 0.5);
    N = floor(stringLength / h);
    h = stringLength / N;
    lambdaSq = (c*c*k*k)/(h*h);
    muSq = (kappa*kappa*k*k)/pow(h,4);

}

void VisualString::excite(int exciteSelection, double velocity, int newWidth) {
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


void VisualString::paint(juce::Graphics& g) {

    Path stringState = visualiseState (g, 100);
    g.setColour(Colours::black); //color of line
    g.strokePath(visualiseState (g, 100), // visualScaling depends on your excitation
                 PathStrokeType(2.0f));
}

Path VisualString::visualiseState (Graphics& g, double visualScaling)
{
    // String-boundaries are in the vertical middle of the component
    double stringBoundaries = getHeight() / 2.0;
    
    // initialise path
    Path stringPath;
    
    // start path
    stringPath.startNewSubPath (0, -uPtr[1][0] * visualScaling + stringBoundaries);
    
    double spacing = getWidth() / static_cast<double>(N);
    double x = spacing;
    
    for (int l = 1; l < N; l++) // if you don't save the boundaries use l < N
    {
        float newY = -uPtr[1][l] * visualScaling + stringBoundaries; // Needs to be -u, because a positive u would visually go down
        if (isnan(x) || isinf(abs(x)) || isnan(uPtr[1][l]) || isinf(abs(uPtr[1][l])))
            std::cout << "Wait" << std::endl;
        
        // if we get NAN values, make sure that we don't get an exception
        if (isnan(newY))
            newY = 0;
        
        stringPath.lineTo (x, newY);
        x += spacing;
    }

    return stringPath;
}

void VisualString::resized() {}



