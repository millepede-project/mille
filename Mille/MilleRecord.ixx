

template <typename indLocalType, typename derLocalType, typename indGlobalType, typename derGlobalType> 
void Mille::MilleRecord::addData_impl(double aMeas, double aErr, 
                MilleArrayView<indLocalType> indLocal, MilleArrayView<derLocalType> derLocal,
                MilleArrayView<indGlobalType>    labGlobal,
                MilleArrayView<derGlobalType> derGlobal){
    std::lock_guard g(m_mx_milleIO); 
    if (indLocal.size() != derLocal.size()){
        std::cerr << " MilleRecord::addData expects local labels and derivatives to have same size, but received "<<indLocal.size()<<" int and "<<derLocal.size()<<" float. Will not add entry. "<<std::endl; 
        return ;   
    }
    if (labGlobal.size() != derGlobal.size()){
        std::cerr << " MilleRecord::addData expects global labels and derivatives to have same size, but received "<<labGlobal.size()<<" int and "<<derGlobal.size()<<" float. Will not add entry. "<<std::endl; 
        return ;   
    }
    // ensure the header is in place 
    newSetIfNeeded(); 
    // add measurement
    buffer_.fill(0, aMeas);
    // add local derivative block
    for (unsigned int i = 0; i < derLocal.size(); ++i){
        if (derLocal[i] != 0 or writeZeroDerivatives_){
            buffer_.fill(indLocal[i], derLocal[i]); 
        }
    }
    // add uncertainty 
    buffer_.fill(0, aErr); 
    // add global derivative block 
    for (std::size_t k = 0; k < derGlobal.size(); ++k){
        if (derGlobal[k] != 0 or writeZeroDerivatives_){
            buffer_.fill(labGlobal[k], derGlobal[k]);
        } 
    }
}
