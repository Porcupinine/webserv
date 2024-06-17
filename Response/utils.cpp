#include "../includes/response.h"

std::string Response::generateAutoIndexPage(const std::string& path, const std::string& host, int port) { // look into this
    std::stringstream html;

    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "<title>Index of " << path << "</title>\n";
    html << "<style>\n";
    html << "body { font-family: Arial, sans-serif; }\n";
    html << "table { width: 100%; border-collapse: collapse; }\n";
    html << "th, td { padding: 8px 12px; border: 1px solid #ddd; text-align: left; }\n";
    html << "th { background-color: #f2f2f2; }\n";
    html << "</style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "<h1>Index of " << path << "</h1>\n";
    html << "<table>\n";
    html << "<tr><th>Name</th><th>Size</th><th>Last Modified</th></tr>\n";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string name = entry.path().filename().string();
        std::string size = entry.is_directory() ? "-" : std::to_string(std::filesystem::file_size(entry.path()));
        std::string last_modified = "-"; // Simplification; use actual timestamp conversion in a real case

        html << "<tr>";
        html << "<td><a href=\"" << name << "\">" << name << "</a></td>";
        html << "<td>" << size << "</td>";
        html << "<td>" << last_modified << "</td>";
        html << "</tr>\n";
    }

    html << "</table>\n";
    html << "</body>\n";
    html << "</html>\n";

    return html.str();
}
