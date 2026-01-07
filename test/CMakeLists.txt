# Unit test configuration for Mille 

add_executable(testMilleRecord testMilleRecord.cc)
target_link_libraries(testMilleRecord PUBLIC Mille2)    
install (
        TARGETS testMilleRecord
        DESTINATION bin
    )
add_test(NAME TestMilleRecord COMMAND testMilleRecord)

add_executable(testMilleRecordC testMilleRecordC.cc)
target_link_libraries(testMilleRecordC PUBLIC Mille2)    
install (
        TARGETS testMilleRecordC
        DESTINATION bin
    )
add_test(NAME TestMilleRecordC COMMAND testMilleRecordC)

add_executable(testMilleRecordCSV testMilleRecordCSV.cc)
target_link_libraries(testMilleRecordCSV PUBLIC Mille2)    
install (
        TARGETS testMilleRecordCSV
        DESTINATION bin
    )
add_test(NAME TestMilleRecordCSV COMMAND testMilleRecordCSV)

add_executable(testMilleReaderC testMilleReaderC.cc)
target_link_libraries(testMilleReaderC PUBLIC Mille2)    
install (
        TARGETS testMilleReaderC
        DESTINATION bin
    )
add_test(NAME TestMilleReaderC COMMAND testMilleReaderC)

add_executable(testMilleReaderCSV testMilleReaderCSV.cc)
target_link_libraries(testMilleReaderCSV PUBLIC Mille2)    
install (
        TARGETS testMilleReaderCSV
        DESTINATION bin
    )
add_test(NAME TestMilleReaderCSV COMMAND testMilleReaderCSV)

add_executable(testMilleDecoder testMilleDecoder.cc)
target_link_libraries(testMilleDecoder PUBLIC Mille2)    
install (
        TARGETS testMilleDecoder
        DESTINATION bin
    )
add_test(NAME TestMilleDecoder COMMAND testMilleDecoder)

add_executable(testMilleFactory testMilleFactory.cc)
target_link_libraries(testMilleFactory PUBLIC Mille2)    
install (
        TARGETS testMilleFactory
        DESTINATION bin
    )
add_test(NAME TestMilleFactory COMMAND testMilleFactory)

if (build_ROOT_comps)
    add_executable(testMilleRecordROOT testMilleRecordROOT.cc)
    target_link_libraries(testMilleRecordROOT PUBLIC Mille2 )    
    install (
            TARGETS testMilleRecordROOT
            DESTINATION bin
        )
    add_test(NAME TestMilleRecordROOT COMMAND testMilleRecordROOT)


    add_executable(testMilleReaderROOT testMilleReaderROOT.cc)
    target_link_libraries(testMilleReaderROOT PUBLIC Mille2 )    
    install (
            TARGETS testMilleReaderROOT
            DESTINATION bin
        )
    add_test(NAME TestMilleReaderROOT COMMAND testMilleReaderROOT)
endif()

if (SUPPORT_ZLIB)
    add_executable(testMilleReaderCcompressed testMilleReaderCcompressed.cc)
    target_link_libraries(testMilleReaderCcompressed PUBLIC Mille2)    
    install (
            TARGETS testMilleReaderCcompressed
            DESTINATION bin
        )
    add_test(NAME TestMilleReaderCcompressed COMMAND testMilleReaderCcompressed)

endif()

add_executable(testMilleC testMilleC.cc)
target_link_libraries(testMilleC PUBLIC MilleC)    
install (
        TARGETS testMilleC
        DESTINATION bin
    )
add_test(NAME TestMilleC COMMAND testMilleC)
