import javax.xml.bind.annotation.*;

@XmlRootElement(name = "физ")
public class Data2 implements Data {
    @XmlElement(name = "иннфиз")
    public int inn;

    @XmlElement(name = "имяфиз")
    public String name;

    @XmlElement(name = "док")
    public Document doc;

    public static class Document {
        @XmlElement(name = "тип")
        public String type;
    }

    @Override
    public String toString() {
        return "Data2{" +
                "inn=" + inn +
                ", name='" + name + '\'' +
                ", doc=" + doc.type +
                '}';
    }
}