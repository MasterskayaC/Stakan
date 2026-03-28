#include <iostream>

#include "app.h"

namespace app {

Application::Application() {
    //заглушка - initialize connection parameters
}

void Application::Run() {
    //заглушка - display menu and handle commands
    std::cout << "Snapshot Console Client\n";
    
    //заглушка - connect to server
    snapshot_client_.connect_to_server("127.0.0.1", 9000);
    
    //заглушка - fetch snapshot
    snapshot_client_.fetch_snapshot();
    
    //заглушка - display real-time updates
    snapshot_client_.display_realtime_updates();
}

} // namespace app
