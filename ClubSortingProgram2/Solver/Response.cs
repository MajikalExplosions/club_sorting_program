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
                throw new SolverException(200, string.Format("File {0} doesn't exist.", fileName));
            }

            //Check for file extension
            string type = fileName.Substring(fileName.LastIndexOf('.') + 1).ToUpper();
            if (fileName.LastIndexOf('.') == -1)
            {
                throw new SolverException(201, string.Format("Missing filename extension."));
            }

            //Read contents
            string contents = File.ReadAllText(fileName, Encoding.UTF8);
            List<List<string>> parsedLines;

            //Parse to lines
            if (type == "CSV")
            {
                parsedLines = _parseToLines(contents, ',');
            }
            else if (type == "TSV")
            {
                parsedLines = _parseToLines(contents, '\t');
            }
            else
            {
                throw new SolverException(202, string.Format("Unsupported file type {0}.", type));
            }

            //Check to make sure there's at least one line so toArray doesn't break
            parsedLines = _padLength(_removeEmpty(parsedLines));
            if (parsedLines.Count == 0)
            {
                throw new SolverException(203, string.Format("File {0} is empty.", fileName));
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
                throw new SolverException(204, string.Format("Could not get row from unread file."));
            }
            if (row > _data.GetLength(0) || row < 0)
            {
                throw new SolverException(205, string.Format("Data row {0} is out of bounds.", row));
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
                throw new SolverException(206, string.Format("Could not get row from unread file."));
            }
            if (col > _data.GetLength(1) || col < 0)
            {
                throw new SolverException(207, string.Format("Data col {0} is out of bounds.", col));
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
                throw new SolverException(208, string.Format("Could not parse incomplete file."));
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
                    throw new SolverException(209, string.Format("Could not convert List<List<string>> to string[,]."));
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