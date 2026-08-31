import java.io.IOException;
import java.util.Map;
import java.util.TreeMap;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.InputSplit;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.MultipleOutputs;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

public class MatrixTranspose {

    public static class TransposeMapper extends Mapper<LongWritable, Text, Text, Text> {

        private String currentFile = null;
        private int br = -1;
        private int bc = -1;
        private int rowIdx = 0;

        @Override
        protected void map(LongWritable key, Text value, Context context)
                throws IOException, InterruptedException {

            InputSplit split = context.getInputSplit();
            if (split instanceof FileSplit) {
                FileSplit fileSplit = (FileSplit) split;
                String fname = fileSplit.getPath().getName();
                if (currentFile == null || !fname.equals(currentFile)) {
                    currentFile = fname;
                    String name = fname.replace(".txt", "");
                    int dash = name.indexOf('-');
                    if (dash != -1) {
                        name = name.substring(0, dash);
                    }
                    String[] parts = name.split("_");
                    br = Integer.parseInt(parts[1]);
                    bc = Integer.parseInt(parts[2]);
                    rowIdx = 0;
                }
            }

            String line = value.toString().trim();
            if (line.isEmpty()) {
                return;
            }

            String[] vals = line.split("\\s+");
            Text outKey = new Text("block_" + bc + "_" + br);

            for (int c = 0; c < vals.length; c++) {
                context.write(outKey, new Text(c + "\t" + rowIdx + "\t" + vals[c]));
            }

            rowIdx++;
        }
    }

    public static class TransposeReducer extends Reducer<Text, Text, NullWritable, Text> {

        private MultipleOutputs<NullWritable, Text> mos;

        @Override
        protected void setup(Context context) {
            mos = new MultipleOutputs<>(context);
        }

        @Override
        protected void reduce(Text key, Iterable<Text> values, Context context)
                throws IOException, InterruptedException {

            TreeMap<Integer, TreeMap<Integer, String>> matrix = new TreeMap<>();

            for (Text val : values) {
                String[] parts = val.toString().split("\t");
                if (parts.length >= 3) {
                    int r = Integer.parseInt(parts[0]);
                    int c = Integer.parseInt(parts[1]);
                    String v = parts[2];

                    matrix.putIfAbsent(r, new TreeMap<>());
                    matrix.get(r).put(c, v);
                }
            }

            String blockName = key.toString();
            for (Map.Entry<Integer, TreeMap<Integer, String>> row : matrix.entrySet()) {
                StringBuilder sb = new StringBuilder();
                boolean first = true;
                for (Map.Entry<Integer, String> col : row.getValue().entrySet()) {
                    if (!first) {
                        sb.append(" ");
                    }
                    sb.append(col.getValue());
                    first = false;
                }
                mos.write(NullWritable.get(), new Text(sb.toString()), blockName);
            }
        }

        @Override
        protected void cleanup(Context context) throws IOException, InterruptedException {
            mos.close();
        }
    }

    public static void main(String[] args) throws Exception {
        if (args.length < 2) {
            System.err.println("Usage: MatrixTranspose <input> <output>");
            System.exit(1);
        }

        Configuration conf = new Configuration();
        Job job = Job.getInstance(conf, "Matrix Transpose");

        job.setJarByClass(MatrixTranspose.class);
        job.setMapperClass(TransposeMapper.class);
        job.setReducerClass(TransposeReducer.class);

        job.setMapOutputKeyClass(Text.class);
        job.setMapOutputValueClass(Text.class);

        job.setOutputKeyClass(NullWritable.class);
        job.setOutputValueClass(Text.class);

        job.setInputFormatClass(TextInputFormat.class);
        job.setOutputFormatClass(TextOutputFormat.class);

        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}
