using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ClubSortingProgram2.Solver
{
    public class Response
    {
        public readonly string FileName;
        private string[,] _data;//[row, col]
        private readonly bool _success;

        public Response(string fileName)
        {
            //It's not successful if it returns early
            _success = false;

            //Check for file existence
            if (!File.Exists(fileName))
            {
                MainScreen.Instance.AlertError("File " + fileName + " doesn't exist.");
                return;
            }

            //Check for file extension
            string type = fileName.Substring(fileName.LastIndexOf('.') + 1).ToUpper();
            if (fileName.LastIndexOf('.') == -1)
            {
                MainScreen.Instance.AlertError("Missing filename extension.");
                return;
            }

            //Read contents
            string contents = File.ReadAllText(fileName, Encoding.UTF8);
            List<List<string>> parsedLines = null;

            //Parse to lines; returns null if file couldn't be parsed
            if (type == "CSV")
            {
                parsedLines = _parseToLines(contents, ',');
            }
            else if (type == "TSV")
            {
                parsedLines = _parseToLines(contents, '\t');
            }
            //TODO Maybe add support for other file types?
            else
            {
                MainScreen.Instance.AlertError("Unsupported file type.");
            }

            //Make sure it was parsed successfully; if it wasn't, errors would have been thrown.
            if (parsedLines == null) return;

            //Check to make sure there's at least one line so toArray doesn't break
            parsedLines = _padLength(_removeEmpty(parsedLines));
            if (parsedLines.Count == 0)
            {
                MainScreen.Instance.AlertError("Empty file.");
                return;
            }
            _data = toArray(parsedLines);
            _success = true;
        }

        public bool Successful()
        {
            return _success;
        }

        public string[] GetRow(int row)
        {
            if (!_success)
            {
                MainScreen.Instance.AlertError("Could not get row from bad file.");
            }
            if (row > _data.GetLength(0) || row < 0)
            {
                MainScreen.Instance.AlertError("Could not get data row " + row.ToString() + ": out of bounds.");
            }

            //Create array
            string[] arr = new string[_data.GetLength(1)];
            for (int i = 0; i < _data.GetLength(1); i++)
            {
                arr[i] = _data[row, i];
            }
            return arr;
        }

        public string[] GetColumn(int col)
        {
            if (!_success)
            {
                MainScreen.Instance.AlertError("Could not get column from bad file.");
            }
            if (col > _data.GetLength(1) || col < 0)
            {
                MainScreen.Instance.AlertError("Could not get data col " + col.ToString() + ": out of bounds.");
            }

            //Create array
            string[] arr = new string[_data.GetLength(0)];
            for (int i = 0; i < _data.GetLength(0); i++)
            {
                arr[i] = _data[i, col];
            }
            return arr;
        }

        private List<List<string>> _parseToLines(string text, char delimiter)
        {
            //Make sure the text ends in a newline
            text += "\n";
            List<List<string>> final = new List<List<string>>();
            final.Add(new List<string>());

            //Keep track of state
            bool inQuotes = false;
            bool escaped = false;
            string currentString = "";

            foreach (char c in text)
            {
                if (escaped)
                {
                    //Escaped characters always go through (like \")
                    currentString += c;
                    escaped = false;
                }
                else if (c == delimiter)
                {
                    //If we find the delimiter, create a new line unless it's in quotes
                    if (inQuotes) currentString += c;
                    else
                    {
                        //Add the current word to the line
                        final[final.Count - 1].Add(currentString.Trim());
                        currentString = "";
                    }
                }
                else if (c == '\\')
                {
                    escaped = true;
                }
                else if (c == '\n')
                {
                    if (inQuotes) currentString += c;
                    else
                    {
                        //New line, so add the last word and create a new line
                        final[final.Count - 1].Add(currentString.Trim());
                        currentString = "";
                        final.Add(new List<string>());
                    }
                }
                else if (c == '\"')
                {
                    //Can't be escaped, so is a normal one
                    inQuotes = !inQuotes;
                }
                else
                {
                    //All other characters
                    currentString += c;
                }
            }
            if (escaped || inQuotes || currentString.Trim().Length != 0)
            {
                MainScreen.Instance.AlertError("Could not parse file (incomplete).");
                return null;
            }
            return final;
        }

        private List<List<string>> _removeEmpty(List<List<string>> arr)
        {
            List<List<string>> final = new List<List<string>>();

            foreach (List<string> l in arr)
            {
                if (l.Count != 0) final.Add(l);
            }

            return final;
        }

        private List<List<string>> _padLength(List<List<string>> arr)
        {
            int max = -1;
            foreach (List<string> l in arr)
            {
                max = Math.Max(l.Count, max);
            }

            foreach (List<string> l in arr)
            {
                while (l.Count < max) l.Add("");
            }

            return arr;
        }

        private string[,] toArray(List<List<string>> lst)
        {
            int numRows = lst.Count;
            int numCols = lst[0].Count;
            string[,] arr = new string[numRows, numCols];
            for (int i = 0; i < numRows; i++)
            {
                if (lst[i].Count != numCols)
                {
                    MainScreen.Instance.AlertError("Could not convert List<List<string>> to string[,].");
                    return null;
                }
                for (int j = 0; j < numCols; j++)
                {
                    arr[i, j] = lst[i][j];
                }
            }

            return arr;
        }
    }
}